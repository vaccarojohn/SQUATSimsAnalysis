FRIDGE=$1
FOLDER=/sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/batch/g4output/angular_study

for dir in $FOLDER/*; do
    NAME="${dir##*/}"
    echo "Processing: $NAME"
    python processCSVBatch.py $dir ${FRIDGE}Output-${NAME}_nt_${FRIDGE} -t 0 59
    python mergeCSVBatch.py $dir ${FRIDGE}Output-${NAME}_nt_${FRIDGE} -t 0 59
    rm ${dir}/${FRIDGE}Output-${NAME}_nt_${FRIDGE}_t*.csv
done

mv /sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/batch/g4output/angular_study/theta*/* /sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/energy_study/angular_study/g4_data/${FRIDGE}