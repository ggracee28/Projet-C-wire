#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl_tree.h"

#define MAX_LINE_LENGTH 1024
#define MAX_FIELD_LENGTH 256

typedef struct {
    char power_plant[MAX_FIELD_LENGTH];
    char hvb_station[MAX_FIELD_LENGTH];
    char hva_station[MAX_FIELD_LENGTH];
    char lv_station[MAX_FIELD_LENGTH];
    char company[MAX_FIELD_LENGTH];
    char individual[MAX_FIELD_LENGTH];
    double capacity;
    double load;
} Record;

static void parse_csv_line(char* line, Record* record) {
    char* token;
    char* rest = line;

    memset(record, 0, sizeof(Record));
    token = strtok_r(rest, ";", &rest);
    if (token) strncpy(record->power_plant, token, MAX_FIELD_LENGTH - 1);

    token = strtok_r(NULL, ";", &rest);
    if (token) strncpy(record->hvb_station, token, MAX_FIELD_LENGTH - 1);

    token = strtok_r(NULL, ";", &rest);
    if (token) strncpy(record->hva_station, token, MAX_FIELD_LENGTH - 1);

    token = strtok_r(NULL, ";", &rest);
    if (token) strncpy(record->lv_station, token, MAX_FIELD_LENGTH - 1);

    token = strtok_r(NULL, ";", &rest);
    if (token) strncpy(record->company, token, MAX_FIELD_LENGTH - 1);

    token = strtok_r(NULL, ";", &rest);
    if (token) strncpy(record->individual, token, MAX_FIELD_LENGTH - 1);

    token = strtok_r(NULL, ";", &rest);
    if (token && strlen(token) > 0) record->capacity = atof(token);

    token = strtok_r(NULL, ";", &rest);
    if (token && strlen(token) > 0) record->load = atof(token);
}

static int is_empty_or_dash(const char* str) {
    return str == NULL || strlen(str) == 0 || strcmp(str, "-") == 0;
}

//// Checks whether the string contains a valid identifier (non-empty and not a hyphen)
static int is_valid_id(const char* str) {
    return str != NULL && strlen(str) > 0 && strcmp(str, "-") != 0;
}

static int line_belongs_to_station_type(const Record* r, const char* station_type) {
    if (strcmp(station_type, "hvb") == 0) {
        // HVB station: doit avoir un ID HVB valide
        return is_valid_id(r->hvb_station);
    } else if (strcmp(station_type, "hva") == 0) {
        // HVA station: doit avoir un ID HVA valide
        return is_valid_id(r->hva_station);
    } else if (strcmp(station_type, "lv") == 0) {
        // LV station: doit avoir un ID LV valide
        return is_valid_id(r->lv_station);
    }
    return 0;
}

static int is_company_only(const Record* r) {
    return is_valid_id(r->company) && is_empty_or_dash(r->individual);
}

static int is_individual_only(const Record* r) {
    return is_valid_id(r->individual) && is_empty_or_dash(r->company);
}

static int is_any_consumer(const Record* r) {
    return is_valid_id(r->company) || is_valid_id(r->individual);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <csv_file> <station_type> <consumer_type>\n", argv[0]);
        return 1;
    }

    char* csv_file = argv[1];
    char* station_type = argv[2];
    char* consumer_type = argv[3];

    FILE* file = fopen(csv_file, "r");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", csv_file);
        return 1;
    }

    Node* root = NULL;
    char line[MAX_LINE_LENGTH];
    Record record;
    int processed_count = 0;

    // Ignore header
    if (!fgets(line, sizeof(line), file)) {
        fprintf(stderr, "Error: empty file or no header.\n");
        fclose(file);
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        parse_csv_line(line, &record);

        if (!line_belongs_to_station_type(&record, station_type)) {
            continue;
        }

        const char* station_id = NULL;
        double capacity = record.capacity;
        double load = 0;

        if (strcmp(station_type, "hvb") == 0) {
            station_id = record.hvb_station;
        } else if (strcmp(station_type, "hva") == 0) {
            station_id = record.hva_station;
        } else if (strcmp(station_type, "lv") == 0) {
            station_id = record.lv_station;
        }

        // Checks whether it is a valid consumption line of the type requested
        if (strcmp(consumer_type, "comp") == 0 && is_company_only(&record)) {
            load = record.load;
            processed_count++;
        } else if (strcmp(consumer_type, "indiv") == 0 && is_individual_only(&record)) {
            load = record.load;
            processed_count++;
        } else if (strcmp(consumer_type, "all") == 0 && is_any_consumer(&record)) {
            load = record.load;
            processed_count++;
        }

        if (is_valid_id(station_id)) {
            root = insert(root, station_id, capacity, load);
        }
    }

    fclose(file);

    // Impression de l'en-tête
    fprintf(stdout, "Station ");
    if (strcmp(station_type, "hvb") == 0) {
        fprintf(stdout, "HVB:Capacité:Consommation (entreprises)\n");
    } else if (strcmp(station_type, "hva") == 0) {
        fprintf(stdout, "HVA:Capacité:Consommation (entreprises)\n");
    } else if (strcmp(station_type, "lv") == 0) {
        if (strcmp(consumer_type, "comp") == 0) {
            fprintf(stdout, "LV:Capacité:Consommation (entreprises)\n");
        } else if (strcmp(consumer_type, "indiv") == 0) {
            fprintf(stdout, "LV:Capacité:Consommation (particuliers)\n");
        } else {
            fprintf(stdout, "LV:Capacité:Consommation (tous)\n");
        }
    }

    inorderTraversal(root, stdout);

    freeTree(root);
    return 0;
}
