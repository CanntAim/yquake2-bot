current=$(pwd)
echo $current
cd ../../../src
grep -rwho '".*.*.md2"' | sed 's/"//g' > ${current}/models.txt
grep -rwho '".*.*.wav"' | sed 's/"//g' > ${current}/sounds.txt
cd $current
