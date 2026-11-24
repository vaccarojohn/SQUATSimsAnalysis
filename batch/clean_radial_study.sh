FRIDGE=$1
FOLDER=/sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/batch/g4output/radial_study

for dir in $FOLDER/*; do
    NAME="${dir##*/}"
    echo "Processing: $NAME"
    python processCSVBatch.py $dir ${FRIDGE}Output-${NAME}_nt_${FRIDGE} -t 0 19
    python mergeCSVBatch.py $dir ${FRIDGE}Output-${NAME}_nt_${FRIDGE} -t 0 19
    rm ${dir}/${FRIDGE}Output-${NAME}_nt_${FRIDGE}_t*.csv
done

mv /sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/batch/g4output/radial_study/theta*/* /sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/energy_study/radial_study/g4_data/${FRIDGE}