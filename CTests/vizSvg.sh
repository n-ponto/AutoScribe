# Takes a SVG file then creates an ncode file and a BMP vizualization of the drawing

image_file=$1
save_file=testSaveFile.ncode

rm $save_file # Remove the previous save file

# Define the commands
parse_svg_command="python ../ImageProcessor/svg_parser.py $image_file $save_file"
viz_command="./draw $save_file"

printf "\nTranslating $image_file into $save_file\n"
if make draw; then

    if $parse_svg_command; then
        printf "\nVizualizing $save_file\n"
        if $viz_command; then
            code $save_file
            code ncode.bmp
            printf "All stages of vizualization succeeded"
        else
            printf "Failed to vizualize $save_file\n"
        fi
    else
        printf 'Failed to parse SVG file\n'
    fi
else
    printf "\nFailed to build draw"
fi
