#include "GeometricTranformer.h"
#include <fstream>

void help();

int main(int argc, char** argv)
{
	AffineTransform x;
	GeometricTransformer gt;
	PixelInterpolate *b = new  BilinearInterpolate();

	Mat src = imread("E:\\3.jpg", IMREAD_COLOR);

	Mat dst;

	gt.RotateKeepImage(src, dst, 30, b);

	imshow("win2", dst);

}


int main2(int argc, char** argv) {
	AffineTransform affineTransform;
	GeometricTransformer geometricTransformer;
	PixelInterpolate *pixelInterpolate;

	string command, interpolateArg, path, cmdArg, cmdArg2;
	short arg;
	int result;

	//path test
	path = "E:\\3.jpg";

	Mat sourceImage;
	Mat destImage;
	Mat histMatrix, histImage;

	if (argc < 5)
	{
		help();
	}
	else {
		command = string(argv[1]);
		interpolateArg = string(argv[2]);
		path = string(argv[2]);
		cmdArg = string(argv[4]);

		if (argc == 6) {
			cmdArg2 = string(argv[5]);

			if (interpolateArg == "--nn") {
				pixelInterpolate = new NearestNeighborInterpolate();
			}
			else if (interpolateArg == "--bl") {
				pixelInterpolate = new BilinearInterpolate();
			}

			sourceImage = imread(path, IMREAD_COLOR);//???
			imshow("Source Image", sourceImage);

			if (command == "--zoom") {
				result = geometricTransformer.Scale(sourceImage, destImage, stof(cmdArg), stof(cmdArg), pixelInterpolate);
			}
			else 		if (command == "--resize") {
				result = geometricTransformer.Scale(sourceImage, destImage, stof(cmdArg), stof(cmdArg), pixelInterpolate);
			}
			else {
				help();
			}

		}
		else if (argc == 5) {
			if (command == "--rotate") {
				result = geometricTransformer.RotateKeepImage(sourceImage, destImage, stof(cmdArg), pixelInterpolate);
			}
			else 			if (command == "--rotateN") {
				result = geometricTransformer.RotateUnkeepImage(sourceImage, destImage, stof(cmdArg), pixelInterpolate);
			}
			else 			if (command == "--flip") {
				result = geometricTransformer.Flip(sourceImage, destImage, stoi(cmdArg), pixelInterpolate);
			}
			else {
				help();
			}
		}
		else { help(); }

	}
	if (destImage.data) {
		imshow("Destination Image", destImage);
		waitKey(0);
	}

	delete pixelInterpolate;
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
