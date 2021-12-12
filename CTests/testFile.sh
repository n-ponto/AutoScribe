# Whole pipeline from starting with an image and showing drawing output

image_file=$1
save_file=testSaveFile.ncode

rm $save_file
python ../ImageProcessor/ncode_generator.py $image_file $save_file 
make draw
./draw $save_file 