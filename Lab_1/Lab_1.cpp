#include "ColorTransformer.h"
#include "Converter.h"	
#include <string>
#include <fstream>

using namespace std;



void help();

void method_1(cv::Mat & image, int n) {
	// get image width
	int width = image.cols;
	// get image height
	int height = image.rows;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// for grayscale image
			if (image.channels() == 1) {
				image.at < uchar >(x, y) = 255;
			}
			// for color image
			else if (image.channels() == 3) {
				cv::Vec3b & pixel = image.at < cv::Vec3b >(x, y);
				// get the first color in this pixel ( blue )
				pixel[0] = 255;
				// get the second color in this pixel ( green )
				pixel[1] = 255;
				// get the last color in this pixel ( red )
				pixel[2] = 255;
			}
		}
	}
}

void colorReduce(cv::Mat &image, int div = 64) {
	int nl = image.rows; // number of lines
	int nc = image.cols * image.channels();
	for (int j = 0; j < nl; j++) {
		// lấy địa chỉ của dòng thứ j
		uchar* data = image.ptr<uchar>(j);
		for (int i = 0; i < nc; i++) {
			// xử lý trên mỗi pixel
			data[i] = data[i] / div * div + div / 2;
		} // end of line
	}
}

int main2(int argc, string argv[])
{
	//Mat image = imread("E:\\4.jpg", IMREAD_COLOR); // Read the file
	Mat image = imread("E:\\4.jpg", IMREAD_GRAYSCALE); // Read the file
	//imwrite("E:\\4.jpg", image);
	ColorTransformer t;
	Converter c;
	imshow(" Show 1", image);
	Mat img2;

	//t.CalcHistogram(image, img2);
	c.Convert(image, img2, 1);
	//t.HistogramEqualization(image, img2);
	imshow(" Show 2", img2);

	waitKey(0);
	return 0;
}



//////////////////////////



int main(int argc, char** argv) {
	string command, path;
	short arg;
	int result;
	ColorTransformer colorTransformer;
	Converter converter;

	//path test
	path = "E:\\3.jpg";

	Mat sourceImage;
	Mat destImage;
	Mat histMatrix, histImage;

	if (argc < 3)
	{
		help();
	}

	else {
		command = string(argv[1]);
		path = string(argv[2]);
		sourceImage = imread(path, IMREAD_COLOR);//???
		//if (sourceImage.data)
		imshow("Source Image", sourceImage);


		if (argc == 3) {

			if (command == "--rgb2gray") {
				result = converter.Convert(sourceImage, destImage, 0);
			}
			else if (command == "--gray2rgb") {
				result = converter.Convert(sourceImage, destImage, 1);

			}
			else if (command == "--rgb2hsv") {
				result = converter.Convert(sourceImage, destImage, 2);

			}
			else if (command == "--hsv2rgb") {
				result = converter.Convert(sourceImage, destImage, 3);

			}
			else if (command == "--hist") {
				result = colorTransformer.CalcHistogram(sourceImage, histMatrix);

			}
			else if (command == "--equalhist") {
				result = colorTransformer.HistogramEqualization(sourceImage, destImage);
			}
			else if (command == "--drawhist") {
				result = colorTransformer.DrawHistogram(histMatrix, histImage);
			}

			else if (command == "--help") {
				help();
			}
			else {
				help();
			}
		}

		else  if (argc == 4)
		{
			arg = stoi(argv[3]);

			if (command == "--bright") {
				result = colorTransformer.ChangeBrighness(sourceImage, destImage, arg);
			}
			else if (command == "--contrast") {
				result = colorTransformer.ChangeContrast(sourceImage, destImage, arg);
			}
			else if (command == "--compare") {
				string imgPath = (string)(argv[3]);
				Mat sourceImage2 = imread(imgPath);
				colorTransformer.CompareImage(sourceImage, sourceImage2);
			}
			else {
				help();
			}

		}
		else {
			help();
		}

	}

	if (destImage.data) {
		imshow("Destination Image", destImage);
		waitKey(0);
	}
	return 1;
}


void help() {

	string line;
	ifstream myfile("help.txt");
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			cout << line << '\n';
		}
		myfile.close();
	} 
	else cout << "Unable to open file";
}