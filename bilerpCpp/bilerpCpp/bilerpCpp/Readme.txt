I created several functions in the cpp file:
getColorFromBitmap, setColorToBitmap and generateFilterImageByRow: you can understand the functions directly through the comments along with the codes.

Also, I used std::thread to realize the multithread, and the array creation of the std::thread requires constant variables, thus I set a const size_t max_num_threads for the threads. 

It supports all texture sizes and resolutions, and I tried to write the code as specific as possible, feel free to contact me with any questions.
