#include"Converter.h"


Converter::Converter() {}
Converter::~Converter() {}

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.data == NULL)
	return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nchan = sourceImage.channels();
	destinationImage = Mat(height, width, CV_8UC1);
	Mat img = Mat(sourceImage);
	for (int y = 0; y < height; y++)
	{
		uchar* prows = img.ptr<uchar>(y);
		uchar* prowd = destinationImage.ptr<uchar>(y);
		for (int x = 0; x < width; x++, prows += nchan, prowd++)
		{
			uchar B = prows[0];
			uchar G = prows[1];
			uchar R = prows[2];
			uchar gray_value = (uchar)(0.11 * B + 0.59 * G + 0.3 * R);
			prowd[0] = gray_value;
		}
	}
	return 0;
}

int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nchan = sourceImage.channels();
	if (nchan > 1) {
		std::cout << "Not Grayscale Image!\n";
		return 1;
	}
	destinationImage = Mat(height, width, CV_8UC3);
	Mat img = Mat(sourceImage);
	for (int y = 0; y < height; y++)
	{
		uchar* prows = img.ptr<uchar>(y);
		uchar* prowd = destinationImage.ptr<uchar>(y);
		for (int x = 0; x < width; x++, prows += nchan, prowd += 3)
		{
			prowd[0] = prows[0];
			prowd[1] = prows[0];
			prowd[2] = prows[0];
		}
	}
	return 0;
}


int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	int nchan = sourceImage.channels();
	destinationImage = Mat(height, width, CV_64FC3);
	Mat img = Mat(sourceImage);
	for (int y = 0; y < height; y++)
	{
		uchar* prows = img.ptr<uchar>(y);
		double* prowd = destinationImage.ptr<double>(y);
		for (int x = 0; x < width; x++, prows += nchan, prowd += 3)
		{
			uchar R = prows[2];
			uchar G = prows[1];
			uchar B = prows[0];
			uchar cmax = max({ R,G,B });
			uchar cmin = min({ R,G,B });
			uchar delta = cmax - cmin;
			double H = 0, S = 0, V = cmax * 1.0 / 255;
			if (delta == 0)
				H = 0;
			else {
				if (cmax == R)
					H = 60.0 * (fmod((G - B) * 1.0 / delta, 6));
				if (cmax == G)
					H = 60.0 * ((B - R) * 1.0 / delta + 2);
				if (cmax == B)
					H = 60.0 * ((R - G) * 1.0 / delta + 4);
			}
			if (cmax == 0)
				S = 0;
			else
				S = delta * 1.0 / cmax;
			prowd[0] = H;
			prowd[1] = S;
			prowd[2] = V;
		}
	}
	return 0;
}


int Converter::HSV2RGB(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.data == NULL)
		return 1;
	int width = sourceImage.cols, height = sourceImage.rows;
	destinationImage = Mat(height, width, CV_8UC3);
	Mat img = Mat(sourceImage);
	for (int y = 0; y < height; y++)
	{
		double* prows = img.ptr<double>(y);
		uchar* prowd = destinationImage.ptr<uchar>(y);
		for (int x = 0; x < width; x++, prows += 3, prowd += 3)
		{
			double H = prows[0];
			double S = prows[1];
			double V = prows[2];
			double C = V * S;
			double X = C * (1 - abs(fmod(H / 60, 2) - 1));
			double m = V - C;
			double r, g, b;
			if (H < 60)
			{
				r = C; g = X; b = 0;
			}
			else
				if (H < 120)
				{
					r = X; g = C; b = 0;
				}
				else
					if (H < 180)
					{
						r = 0; g = C; b = X;
					}
					else
						if (H < 240)
						{
							r = 0; g = X; b = C;
						}
						else
							if (H < 300)
							{
								r = X; g = 0; b = C;
							}
							else
							{
								r = C; g = 0; b = X;
							}
			uchar R = (uchar)(255 * (r + m));
			uchar G = (uchar)(255 * (g + m));
			uchar B = (uchar)(255 * (b + m));
			prowd[0] = B;
			prowd[1] = G;
			prowd[2] = R;
		}
	}
	return 0;
}


int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type) {
	if (type == 0)
		Converter::RGB2GrayScale(sourceImage, destinationImage);
	else
		if (type == 1)
			Converter::GrayScale2RGB(sourceImage, destinationImage);
		else
			if (type == 2)
				Converter::RGB2HSV(sourceImage, destinationImage);
			else
				if (type == 3)
					Converter::HSV2RGB(sourceImage, destinationImage);
				else
					return 1;
	return 0;
}