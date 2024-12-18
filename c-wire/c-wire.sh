#!/bin/bash

show_help() {
    echo "Usage: $0 [-h] <csv_file> <station_type> <consumer_type> [power_plant_id]"
    echo
    echo "Parameters:"
    echo "  -h                   Show this help and exit."
    echo "  <csv_file>           Path to the CSV input file."
    echo "  <station_type>       One of: hvb, hva, lv"
    echo "  <consumer_type>      One of: comp, indiv, all"
    echo "  [power_plant_id]     Optional ID of a specific power plant to filter on."
    echo
    echo "Notes:"
    echo "  Invalid combinations:"
    echo "    - HVB cannot have 'indiv' or 'all' consumers."
    echo "    - HVA cannot have 'indiv' or 'all' consumers."
    echo
    echo "Examples:"
    echo "  $0 input/data.csv hvb comp"
    echo "  $0 input/data.csv hva comp 3"
    echo "  $0 input/data.csv lv all"
    exit 0
}

# Check if help is requested
if [ "$1" == "-h" ]; then
    show_help
fi

if [ $# -lt 3 ]; then
    echo "Error: Not enough arguments."
    show_help
fi

CSV_FILE=$1
STATION_TYPE=$2
CONSUMER_TYPE=$3
POWER_PLANT_ID=$4

# Validate station type
if [[ ! $STATION_TYPE =~ ^(hvb|hva|lv)$ ]]; then
    echo "Error: Invalid station type. Must be hvb, hva, or lv."
    show_help
fi

# Validate consumer type
if [[ ! $CONSUMER_TYPE =~ ^(comp|indiv|all)$ ]]; then
    echo "Error: Invalid consumer type. Must be comp, indiv, or all."
    show_help
fi

# Check invalid combinations
if [ "$STATION_TYPE" == "hvb" ] && ([ "$CONSUMER_TYPE" == "indiv" ] || [ "$CONSUMER_TYPE" == "all" ]); then
    echo "Error: Invalid combination: HVB stations cannot have indiv or all consumers."
    show_help
fi

if [ "$STATION_TYPE" == "hva" ] && ([ "$CONSUMER_TYPE" == "indiv" ] || [ "$CONSUMER_TYPE" == "all" ]); then
    echo "Error: Invalid combination: HVA stations cannot have indiv or all consumers."
    show_help
fi

# Check CSV file existence
if [ ! -f "$CSV_FILE" ]; then
    echo "Error: File $CSV_FILE does not exist."
    exit 1
fi

# Check CSV file non-empty
file_size=$(stat -c%s "$CSV_FILE" 2>/dev/null)
if [ "$file_size" -eq 0 ]; then
    echo "Error: File $CSV_FILE is empty."
    exit 1
fi

echo "Processing file: $CSV_FILE ($(($file_size/1024/1024)) MB)"

# Create/clean directories
mkdir -p tmp graphs output
rm -f tmp/*

# Move to codeC directory for compilation
cd codeC || exit 1
make clean && make
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    cd ..
    exit 1
fi
cd ..

START_TIME=$(date +%s.%N)

# Filter data by power plant if specified
if [ -n "$POWER_PLANT_ID" ]; then
    # Keep header + filter lines
    head -n 1 "$CSV_FILE" > tmp/filtered.csv
    awk -F';' -v plant="$POWER_PLANT_ID" 'NR>1 && $1==plant' "$CSV_FILE" >> tmp/filtered.csv
else
    cp "$CSV_FILE" tmp/filtered.csv
fi

linecount=$(wc -l < tmp/filtered.csv)
if [ "$linecount" -le 1 ]; then
    echo "Warning: No data after filtering."
fi

# Determine output filename base
OUTFILE_BASE="${STATION_TYPE}_${CONSUMER_TYPE}"
if [ -n "$POWER_PLANT_ID" ]; then
    OUTFILE_BASE="${OUTFILE_BASE}_${POWER_PLANT_ID}"
fi

LOGFILE="output/${OUTFILE_BASE}.log"
OUTCSV="output/${OUTFILE_BASE}.csv"

echo "Processing data..."
./codeC/c-wire tmp/filtered.csv "$STATION_TYPE" "$CONSUMER_TYPE" > "$OUTCSV" 2> "$LOGFILE"

RETVAL=$?
END_TIME=$(date +%s.%N)
DURATION=$(echo "$END_TIME - $START_TIME" | bc)

if [ $RETVAL -ne 0 ]; then
    echo "Error during data processing."
    echo "Execution time: ${DURATION}s"
    cat "$LOGFILE"
    exit $RETVAL
fi

echo "Execution time: ${DURATION}s"

if [ -s "$OUTCSV" ]; then
    # Sort by capacity ascending (column 2)
    SORTED="output/${OUTFILE_BASE}_sorted.csv"
    head -n 1 "$OUTCSV" > "$SORTED"
    tail -n +2 "$OUTCSV" | sort -t: -k2,2n >> "$SORTED"

    # If lv all, create lv_all_minmax.csv
    if [ "$STATION_TYPE" == "lv" ] && [ "$CONSUMER_TYPE" == "all" ]; then
        MINMAX="output/lv_all_minmax.csv"
        if [ -n "$POWER_PLANT_ID" ]; then
            MINMAX="output/lv_all_minmax_${POWER_PLANT_ID}.csv"
        fi

        head -n 1 "$SORTED" > tmp/header_minmax.txt
        tail -n +2 "$SORTED" | awk -F':' '{
            diff=$3-$2; if(diff<0) diff=-diff;
            print $0":"diff
        }' | sort -t: -k4,4nr > tmp/lv_all_diff_sorted.csv

        head -n 10 tmp/lv_all_diff_sorted.csv > tmp/top10.csv
        tail -n 10 tmp/lv_all_diff_sorted.csv > tmp/bottom10.csv

        cat tmp/header_minmax.txt > "$MINMAX"
        # Remove the diff column
        cat tmp/top10.csv tmp/bottom10.csv | cut -d: -f1-3 >> "$MINMAX"

        # If gnuplot is available, generate a graph
        if command -v gnuplot &> /dev/null; then
            # Prepare data for gnuplot
            tail -n +2 "$MINMAX" | awk -F':' '{
                diff=$3-$2;
                print $1":"$2":"$3":"diff
            }' > tmp/lv_graph_data.csv

            awk -F':' 'BEGIN{i=0}{i++; print i":"$1":"$2":"$3":"$4}' tmp/lv_graph_data.csv > tmp/lv_graph_data_idx.csv

            gnuplot <<- EOF
                set terminal png size 1024,600
                set output "graphs/lv_consumption.png"
                set title "Top 10 Most and Least Loaded LV Stations"
                set style data histograms
                set style fill solid
                set boxwidth 0.5
                set xtics rotate by -45
                set key off
                plot 'tmp/lv_graph_data_idx.csv' using 1:($4+($3-$2)):(4) with boxes lc variable title "Consumption"
EOF
        else
            echo "Warning: gnuplot not found, skipping graph generation"
        fi
    fi
else
    echo "Warning: No output was generated."
fi

echo "Check $LOGFILE for execution details."
echo "Done!"
