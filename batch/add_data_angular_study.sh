FRIDGE=$1
OFFSET=$2
FOLDER=/sdf/home/j/jvaccaro/DMQIS/SQUATSimsAnalysis/energy_study/angular_study/g4_data

for dir in $(find ${FOLDER}/${FRIDGE}/* -type f ! -iname "*_nt_*"); do
    NAME="${dir##*/}"
    echo "Processing: $NAME"
    python eventOffsetBatch.py ${FOLDER}/${FRIDGE} ${NAME%.*}_nt_${FRIDGE} ${OFFSET}
    python mergeCSVBatch.py ${FOLDER}/${FRIDGE} -f ${NAME%.*} ${NAME%.*}_nt_${FRIDGE} ${NAME%.*}
    rm ${dir}/${NAME%.*}_nt_${FRIDGE}.csv
done
