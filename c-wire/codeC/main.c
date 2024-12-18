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

// Vérification des lignes appartenant réellement au type de station demandé
// hvb: hvb_station non vide, hva_station et lv_station vides, c'est-à-dire juste hvb.
// hva: hva_station non vide, lv_station vide.
// lv : lv_station non vide (postes LV ou conso LV), on ne filtre pas sur hva_station pour LV car un poste LV est alimenté par une station hva. Le critère principal est la présence de lv_station.
static int line_belongs_to_station_type(const Record* r, const char* station_type) {
    if (strcmp(station_type, "hvb") == 0) {
        // HVB station or consumer: hvb_station must not be empty
        // and hva_station, lv_station should ideally be empty. 
        // Toutefois, selon l'exemple, un consommateur HVB a hvb et pas hva ni lv.
        if (strlen(r->hvb_station) > 0 && strlen(r->hva_station) == 0 && strlen(r->lv_station) == 0) {
            return 1;
        } else {
            return 0;
        }
    } else if (strcmp(station_type, "hva") == 0) {
        // HVA station or consumer: hva_station not empty, lv_station empty
        if (strlen(r->hva_station) > 0 && strlen(r->lv_station) == 0) {
            return 1;
        } else {
            return 0;
        }
    } else if (strcmp(station_type, "lv") == 0) {
        // LV station or consumer: lv_station not empty
        // Un poste LV a hv_a et lv, un consommateur LV a lv non vide. 
        // On se base sur lv_station non vide simplement.
        if (strlen(r->lv_station) > 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

// Conditions sur les consommateurs
static int is_company_only(const Record* r) {
    return (strlen(r->company) > 0 && strlen(r->individual) == 0);
}

static int is_individual_only(const Record* r) {
    return (strlen(r->individual) > 0 && strlen(r->company) == 0);
}

static int is_any_consumer(const Record* r) {
    return (strlen(r->company) > 0 || strlen(r->individual) > 0);
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
            continue; // ignorer les lignes qui ne correspondent pas au type de station
        }

        const char* station_id = NULL;
        double capacity = 0;
        double load = 0;

        if (strcmp(station_type, "hvb") == 0) {
            station_id = (strlen(record.hvb_station) > 0) ? record.hvb_station : NULL;
            capacity = record.capacity;
            // hvb comp seulement
            if (strcmp(consumer_type, "comp") == 0 && is_company_only(&record) && record.load > 0) {
                load = record.load;
                processed_count++;
            }
        } else if (strcmp(station_type, "hva") == 0) {
            station_id = (strlen(record.hva_station) > 0) ? record.hva_station : NULL;
            capacity = record.capacity;
            // hva comp seulement
            if (strcmp(consumer_type, "comp") == 0 && is_company_only(&record) && record.load > 0) {
                load = record.load;
                processed_count++;
            }
        } else if (strcmp(station_type, "lv") == 0) {
            station_id = (strlen(record.lv_station) > 0) ? record.lv_station : NULL;
            capacity = record.capacity;
            if (strcmp(consumer_type, "comp") == 0 && is_company_only(&record) && record.load > 0) {
                load = record.load;
                processed_count++;
            } else if (strcmp(consumer_type, "indiv") == 0 && is_individual_only(&record) && record.load > 0) {
                load = record.load;
                processed_count++;
            } else if (strcmp(consumer_type, "all") == 0 && is_any_consumer(&record) && record.load > 0) {
                load = record.load;
                processed_count++;
            }
        }

        if (station_id != NULL && strlen(station_id) > 0) {
            // On insère même si load=0, car on a besoin de la capacité
            // pour que la station soit connue.
            // Le chargement se cumulera sur de multiples insertions du même ID.
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
