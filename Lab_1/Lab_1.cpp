#include "ColorTransformer.h"
#include "Converter.h"	
#include <string>
#include <fstream>

using namespace std;



void help();
int CalculateHistogram(const Mat& sourceImage, Mat& destinationImage)
{
	int histSize = 256;
	Mat histImg;

	// Ảnh xám
	if (sourceImage.channels() == 1)
	{
		// Khởi tạo ảnh chứa histogram
		Mat temp(histSize, 1, CV_32FC1, Scalar(0));

		// Thống kê số lượng điểm ảnh ở mỗi bins
		int nRows = sourceImage.rows, nCols = sourceImage.cols;
		for (int y = 0; y < nRows; y++)
		{
			const uchar* pRow = sourceImage.ptr<uchar>(y);
			for (int x = 0; x < nCols; x++)
			{
				int index = int(pRow[x]);
				temp.ptr<float>(index)[0]++;
			}
		}

		// Sao chép ảnh temp sang ảnh histImg
		temp.copyTo(histImg);
	}
	else if (sourceImage.channels() == 3) // Ảnh màu
	{
		// Khởi tạo ảnh chứa histogram
		Mat temp(histSize, 1, CV_32FC3, Scalar(0, 0, 0));

		// Thống kê số lượng điểm ảnh ở mỗi bins
		int nRows = sourceImage.rows, nCols = sourceImage.cols, nChannels = sourceImage.channels();
		for (int y = 0; y < nRows; y++)
		{
			const uchar* pRow = sourceImage.ptr<uchar>(y);
			for (int x = 0; x < nCols; x++, pRow += nChannels)
			{
				temp.ptr<float>(int(pRow[0]))[0]++;
				temp.ptr<float>(int(pRow[1]))[1]++;
				temp.ptr<float>(int(pRow[2]))[2]++;
			}
		}

		// Sao chép ảnh temp sang ảnh histImg
		temp.copyTo(histImg);
	}
	else {
		return 0;
	}

	// Sao chép dữ liệu ảnh histogram sang ảnh output.
	histImg.copyTo(destinationImage);
	return 1;
}

void printMat(Mat m) {
	for (int i = 0; i < m.rows; i++) {
		for (int j = 0; j < m.cols; j++) {
			cout << m.at<int>(i, j)<<"\t";
		}
		cout << "\n";
	}
}

int main(int argc, string argv[])
{
	Mat image = imread("E:\\3.jpg", IMREAD_COLOR); // Read the file
	//Mat image = imread("E:\\3.jpg", IMREAD_GRAYSCALE); // Read the file
	//imwrite("E:\\4.jpg", image);

	ColorTransformer t;
	Converter c;
	imshow(" Show 1", image);
	Mat img2;
	t.HistogramEqualization(image, img2);
	imshow(" Show 2", img2);


	//t.CalcHistogram(image, img2);
	//t.DrawHistogram(image, img2);

	//printMat(img2);

	waitKey(0);
	return 0;
}



int main2(int argc, char** argv) {
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