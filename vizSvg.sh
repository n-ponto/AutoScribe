# Takes a SVG file then creates an ncode file and a BMP vizualization of the drawing

image_file=$1
save_file=out/vizSvg.ncode

# Remove the previous save file
rm $save_file

# Define the commands
parse_svg_command="python ./ImageProcessor/svg_parser.py $image_file $save_file"
viz_command="python ./Tools/NcodeVizualizer.py $save_file"

printf "Translating $image_file into $save_file\n"
if (cd CTests && make draw); then
    if $parse_svg_command; then  # Turn the SVG into ncode
        printf "\nVizualizing $save_file\n"
        if $viz_command; then  # Vizualize the ncode
            # code $save_file  # Open the ncode file
            code out/vizSvg.bmp  # Open the viz image
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
