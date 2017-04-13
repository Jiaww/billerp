#include <string>
#include <stdint.h>
#include <math.h>
#include <thread>

const size_t BYTES_PER_PIXEL = 3; // 24 bit color depth :D
const size_t max_num_threads = 64; //Max number of the threads can be

//Function Declearations
void writeBitmap(std::string in_filename, size_t in_width, size_t in_height, unsigned char * in_data);
void generateLowResBitmap(size_t in_width, size_t in_height, unsigned char * in_data);
size_t getColorFromBitmap(size_t in_width, size_t in_height, const unsigned char * in_source, size_t h_num, size_t w_num, size_t RGB);
void setColorToBitmap(size_t color_data, size_t in_dest_width, size_t in_dest_height, unsigned char * in_dest, size_t h_num, size_t w_num, size_t RGB);
void generateFilterImageByRow(int start_pixel, int end_pixel, size_t in_src_width, size_t in_src_height, const unsigned char * in_source, size_t in_dest_width, size_t in_dest_height, unsigned char * in_dest);

void generateFilteredImage(const int in_num_threads, size_t in_src_width, size_t in_src_height, const unsigned char * in_source, size_t in_dest_width, size_t in_dest_height, unsigned char * in_dest)
{
	// Start in_num_threads threads to filter one row at a time until the result image is complete.
	// Each thread should:
	// - Figure out the next row in the result image that needs to be computed
	// - Determine the color for each pixel in that row
	// - Set those pixels in in_dest
	// - Keep doing that until all the rows in the bitmap are complete
	std::thread t[max_num_threads];
	for (int i = 0; i < in_num_threads; i++) {
		t[i] = std::thread(generateFilterImageByRow, i*(in_dest_width/in_num_threads), (i+1)*(in_dest_width / in_num_threads), in_src_width, in_src_height, in_source, in_dest_width, in_dest_height, in_dest);
	}
	for (int i = 0; i < in_num_threads; i++) {
		t[i].join();
	}
}
//
void generateFilterImageByRow(int start_pixel, int end_pixel, size_t in_src_width, size_t in_src_height, const unsigned char * in_source, size_t in_dest_width, size_t in_dest_height, unsigned char * in_dest)
{
	//Each thread will deal with one part of the row, and keep looping until all rows are done for this part
	for (int i = 0; i < in_dest_height; i++) {
		for (int j = start_pixel; j < end_pixel; j++) {
			
			if (i == 180 && j == 30) {
				int a = 10;
			}
			int min_height_num = floor((double(i) / in_dest_height) * (in_src_height)-0.5);
			int min_width_num = floor((double(j) / in_dest_width) * (in_src_width)-0.5);

			double height_ratio = (double(i) / in_dest_height) * (in_src_height)-min_height_num - 0.5;
			double width_ratio = (double(j) / in_dest_width) * (in_src_width)-min_width_num - 0.5;

			if (min_height_num < 0) {
				height_ratio = 1.0;
			}
			if (min_width_num < 0) {
				width_ratio = 1.0;
			}
			if (min_height_num >= int(in_src_height-1)) {
				height_ratio = 0.0;
			}
			if (min_width_num >= int(in_src_width-1)) {
				width_ratio = 0.0;
			}

			//Bilinear Interpolation
			//c00
			size_t c00_b = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num, min_width_num, 0);
			size_t c00_g = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num, min_width_num, 1);
			size_t c00_r = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num, min_width_num, 2);
			//c01
			size_t c01_b = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num, min_width_num + 1, 0);
			size_t c01_g = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num, min_width_num + 1, 1);
			size_t c01_r = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num, min_width_num + 1, 2);
			//c10
			size_t c10_b = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num + 1, min_width_num, 0);
			size_t c10_g = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num + 1, min_width_num, 1);
			size_t c10_r = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num + 1, min_width_num, 2);
			//c11
			size_t c11_b = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num + 1, min_width_num + 1, 0);
			size_t c11_g = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num + 1, min_width_num + 1, 1);
			size_t c11_r = getColorFromBitmap(in_src_width, in_src_height, in_source, min_height_num + 1, min_width_num + 1, 2);

			//c0
			size_t c0_b = c00_b * (1.0 - width_ratio) + c01_b * width_ratio;
			size_t c0_g = c00_g * (1.0 - width_ratio) + c01_g * width_ratio;
			size_t c0_r = c00_r * (1.0 - width_ratio) + c01_r * width_ratio;
			//c1
			size_t c1_b = c10_b * (1.0 - width_ratio) + c11_b * width_ratio;
			size_t c1_g = c10_g * (1.0 - width_ratio) + c11_g * width_ratio;
			size_t c1_r = c10_r * (1.0 - width_ratio) + c11_r * width_ratio;

			//final color
			size_t c_b = c0_b * (1 - height_ratio) + c1_b * (height_ratio);
			size_t c_g = c0_g * (1 - height_ratio) + c1_g * (height_ratio);
			size_t c_r = c0_r * (1 - height_ratio) + c1_r * (height_ratio);

			//set color
			setColorToBitmap(c_b, in_dest_width, in_dest_height, in_dest, i, j, 0);
			setColorToBitmap(c_g, in_dest_width, in_dest_height, in_dest, i, j, 1);
			setColorToBitmap(c_r, in_dest_width, in_dest_height, in_dest, i, j, 2);
		}
	}
}

size_t getColorFromBitmap(size_t in_width, size_t in_height, const unsigned char * in_source, size_t h_num, size_t w_num, size_t RGB) {
	//RGB = 0: Blue; RGB = 1: Green; RGB = 2: Red
	int val = h_num*in_width + w_num; 
	int offset = val*BYTES_PER_PIXEL;

	if (RGB == 0) {
		//blue
		return size_t(in_source[offset + 0]);
	}
	else if (RGB == 1) {
		//green
		return size_t(in_source[offset + 1]);
	}
	else if (RGB == 2) {
		//red
		return size_t(in_source[offset + 2]);
	}
}

void setColorToBitmap(size_t color_data, size_t in_dest_width, size_t in_dest_height, unsigned char * in_dest, size_t h_num, size_t w_num, size_t RGB) {
	//RGB = 0: Blue; RGB = 1: Green; RGB = 2: Red
	int val = h_num*in_dest_width + w_num;
	int offset = val*BYTES_PER_PIXEL;
	if (RGB == 0) {
		//blue
		in_dest[offset + 0] = color_data;
	}
	else if (RGB == 1) {
		//green
		in_dest[offset + 1] = color_data;
	}
	else if (RGB == 2) {
		//red
		in_dest[offset + 2] = color_data;
	}
}

#pragma region main
int main(int argc, char ** argv)
{
	const size_t result_height = 512;
	const size_t result_width = 512;
	const size_t generated_height = 3;
	const size_t generated_width = 3;

	// Generate a 3x3 lowres image
	// Use Bilinear Interpolation to create a larger version (512x512)
	// Write the result image out to "bilerp.bmp"

	unsigned char * generated_image = new unsigned char[generated_width*generated_height*BYTES_PER_PIXEL];
	memset(generated_image,0,generated_width*generated_height*BYTES_PER_PIXEL);

	generateLowResBitmap(generated_width,generated_height,generated_image);

	unsigned char * result_image = new unsigned char[result_width*result_height*BYTES_PER_PIXEL];
	memset(result_image,0,result_width*result_height*BYTES_PER_PIXEL);

	generateFilteredImage(8,generated_width, generated_height, generated_image, result_width, result_height, result_image);
	
	writeBitmap("generated.bmp", generated_width, generated_height, generated_image);

	delete[] generated_image;

	writeBitmap("bilerp.bmp",result_width,result_height,result_image);

	delete[] result_image;

	return 0;
}
#pragma endregion

#pragma region Lowres Bitmap Creator
void generateLowResBitmap(size_t in_width, size_t in_height, unsigned char * in_data)
{
	for(size_t y = 0; y < in_height; ++y)
	{
		for(size_t x = 0; x < in_width; ++x)
		{
			int val = y*in_width+x; // Used to generate the color
			int offset = val*BYTES_PER_PIXEL;
			in_data[offset+0] = 0;
			in_data[offset+1] = 0;
			in_data[offset+2] = 0;
			if((val&1)==1)
				in_data[offset+0] = 255; // Blue
			if((val&2)==2)
				in_data[offset+1] = 255; // Green
			if((val&4)==4)
				in_data[offset+2] = 255; // Red
		}
	}
}
#pragma endregion

#pragma region Bitmap Writer
#pragma pack(push, 2)
struct BmpFileHeader {
	char magic[2];
	uint32_t filesize;
	uint16_t reserved_1;
	uint16_t reserved_2;
	uint32_t offset; // of the data partition
};

struct BmpInfoHeader {
	uint32_t size; // of the BmpInfoHeader
	uint32_t width;
	uint32_t height;
	uint16_t num_color_planes; // Must be 1
	uint16_t depth;
	unsigned char unused[24]; // This is not functionality we're using here :)
};
#pragma pack(pop)

void writeBitmap(std::string in_filename, size_t in_width, size_t in_height, unsigned char * in_data)
{
	size_t filesize = 54 + BYTES_PER_PIXEL*in_width*in_height; // Header + 3 Color

	// Using C style files here because fwrite is more conducive to this than ofstream::write
	//  since it has a count (used in the padding portion)
	FILE * f = NULL;
#ifdef _MSC_VER
	errno_t err = fopen_s(&f,in_filename.c_str(),"wb");
	if(f==NULL || err != 0)
		return;
#else//_MSC_VER
	f = fopen(in_filename.c_str(),"wb");
	if(f==NULL)
		return;
#endif//_MSC_VER

	struct BmpFileHeader file_header;
	memset(&file_header,0,sizeof(struct BmpFileHeader));
	file_header.magic[0] = 'B'; file_header.magic[1] = 'M';
	file_header.offset = sizeof(struct BmpFileHeader) + sizeof(BmpInfoHeader);
	fwrite(&file_header,1,sizeof(struct BmpFileHeader),f);

	struct BmpInfoHeader info_header;
	memset(&info_header,0,sizeof(struct BmpInfoHeader));
	info_header.size = sizeof(struct BmpInfoHeader);
	info_header.width = in_width;
	info_header.height = in_height;
	info_header.num_color_planes = 1;
	info_header.depth = 24;
	fwrite(&info_header,1,sizeof(struct BmpInfoHeader),f);

	unsigned char padding[] = {0,0,0,};
	for(size_t i=0;i<in_height;++i)
	{
		// in_height-i-1 because the scanlines are bottom up in a bitmap :)
		fwrite(in_data+(in_width)*(in_height-i-1)*BYTES_PER_PIXEL,BYTES_PER_PIXEL,in_width,f);
		// The rows need to be 4 byte aligned
		fwrite(padding,1,(4-(in_width*BYTES_PER_PIXEL)%4)%4,f);
	}

	fclose(f);
	return;
}
#pragma endregion