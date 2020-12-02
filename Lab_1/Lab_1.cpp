#include "ColorTransformer.h"
#include "Converter.h"	
#include <string>
#include <fstream>
#include<iomanip>

using namespace std;

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

void showImage(Mat img) {
	if (img.data) {
		imshow("Destination Image", img);
		waitKey(0);
	}
}

bool isGrayImage(Mat img) // returns true if the given 3 channel image is B = G = R
{
	Mat dst;
	Mat bgr[3];

	//tach thanh 3 kenh mau
	split(img, bgr);

	//sai so giua 2 array
	absdiff(bgr[0], bgr[1], dst);

	//co gia tri khac biet --> anh mau
	if (countNonZero(dst))
		return false;

	absdiff(bgr[0], bgr[2], dst);

	return !countNonZero(dst);
}

void readImage(string path, Mat& img) {

	Mat temp = imread(path);

	if (isGrayImage(temp)) {
		img = imread(path, IMREAD_GRAYSCALE);
	}
	else {
		img = imread(path, IMREAD_COLOR);
	}
}


int main(int argc, char** argv) {
	string command, path;
	short arg;
	int result;
	ColorTransformer colorTransformer;
	Converter converter;

	Mat  sourceImage;
	Mat destImage;
	Mat histMatrix, histImage;

	if (argc < 3)
	{
		help();
	}

	else {
		command = string(argv[1]);
		path = string(argv[2]);


		readImage(path, sourceImage);
		imshow("Source Image", sourceImage);


		if (argc == 3) {

			if (command == "--rgb2gray") {
				result = converter.Convert(sourceImage, destImage, 0);
				showImage(destImage);
			}
			else if (command == "--gray2rgb") {
				result = converter.Convert(sourceImage, destImage, 1);
				showImage(destImage);

			}
			else if (command == "--rgb2hsv") {
				result = converter.Convert(sourceImage, destImage, 2);
				showImage(destImage);

			}
			else if (command == "--hsv2rgb") {
				result = converter.Convert(sourceImage, destImage, 3);
				//showImage(destImage);

			}
			else if (command == "--hist") {
				result = colorTransformer.CalcHistogram(sourceImage, histMatrix);

				for (int i = 0; i < histMatrix.rows; i++) {
					for (int j = 0; j < histMatrix.cols; j++) {
						std::cout << "(" << setw(3) << left<< i << " , " << j << "): " << setw(15)<< left << histMatrix.at<int>(i, j);
					}
					std::cout << "\n";
				}

				showImage(sourceImage);
				
			}
			else if (command == "--equalhist") {
				result = colorTransformer.HistogramEqualization(sourceImage, destImage);
				showImage(destImage);
			}
			else if (command == "--drawhist") {
				colorTransformer.CalcHistogram(sourceImage, histMatrix);
				result = colorTransformer.DrawHistogram(histMatrix, destImage);
				showImage(destImage);
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
				showImage(destImage);

			}
			else if (command == "--contrast") {
				result = colorTransformer.ChangeContrast(sourceImage, destImage, arg);
				showImage(destImage);

			}
			else if (command == "--compare") {
				cout << "here12" << endl;

				string path2 = (string)(argv[3]);
				cout << path2 << endl;

				//Mat sourceImage2 ;
				//readImage(path2, sourceImage2);
				//imshow("sdf", sourceImage2);
				//waitKey(0);
				//cout<<colorTransformer.CompareImage(sourceImage, sourceImage2);
			}
			else {
				help();
			}
		}
		else {
			help();
		}
	}

	return 1;
}


