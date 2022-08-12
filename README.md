# AutoScribe

AutoScribe is a drawing machine capable of making any piece of art that can be drawn with pen on paper. On a PC, it converts scalable vector graphic (SVG) images into a set of points which are sent to the Arduino and drawn. 

TODO: include a link to a video

## Installation

<details open>
<summary>Specs</summary>
Windows 10
Python 3.9.12
</details>

First clone the repo:

`git clone https://github.com/n-ponto/AutoScribe.git`

Then install the Python modules

`pip install -r requirements.txt`

<details>
<summary>Why not Anaconda?</summary>
<br>
I recommend not using Anaconda because there are some issues with installing the bezier module within the Anaconda environment. This is only required for the ParseSvg.py script, so if you don't plan to create any of your own images and just rely on the ncode files already provided in this repo, then there's no need to install the bezier module.
</details>
