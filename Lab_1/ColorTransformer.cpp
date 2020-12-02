#include "ColorTransformer.h"
#include"Converter.h"
#include <vector>

ColorTransformer::ColorTransformer()
{
}


ColorTransformer::~ColorTransformer()
{
}
int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b) {
	if (sourceImage.data == NULL)
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int srcChannels = sourceImage.channels();

	if (srcChannels == 1) {
		destinationImage = cv::Mat(height, width, CV_8UC1);
	}
	else {
		destinationImage = cv::Mat(height, width, CV_8UC3);
	}

	int dstChannels = destinationImage.channels();
	std::cout << "So kenh mau: " << dstChannels << "\n";
	int srcWidthStep = sourceImage.step[0];
	int dstWidthStep = destinationImage.step[0];

	std::cout << srcWidthStep << "\n";
	//3*853 = 2559
	std::cout << dstWidthStep;

	uchar* pSrcData = sourceImage.data;
	uchar* pDstData = destinationImage.data;

	for (int y = 0; y < height; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep) {
		uchar* pSrcRow = pSrcData;
		uchar* pDstRow = pDstData;
		for (int x = 0; x < width; x++, pSrcRow += srcChannels, pDstRow += dstChannels) {
			for (int i = 0; i < srcChannels; i++) {
				// saturate_cast<uchar> lam tron pixel ve [0..255] dung nhat vd 257->1 (sai) 
				// => 257->255 (dung) 
				pDstRow[i] = saturate_cast<uchar>(pSrcRow[i] + b);
			}
		}
	}

	return 1;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix) {
	if (sourceImage.data == NULL)
		return 0;

	int width = sourceImage.cols, height = sourceImage.rows;
	int srcChannels = sourceImage.channels();

	//Ma tran 256 x nchannel
	histMatrix = Mat::zeros(256, srcChannels, CV_32S);

	int srcWidthStep = sourceImage.step[0];
	uchar* pSrcData = sourceImage.data;

	for (int y = 0; y < height; y++, pSrcData += srcWidthStep) {
		uchar* pSrcRow = pSrcData;
		for (int x = 0; x < width; x++, pSrcRow += srcChannels) {

			for (int i = 0; i < srcChannels; i++) {
				int binIdx = (int)pSrcRow[i];
				histMatrix.at<int>(binIdx, i)++;
			}
		}
	}

	//for (int i = 0; i < histMatrix.rows; i++) {
	//	for (int j = 0; j < histMatrix.cols; j++) {
	//		std::cout << "\t" << "(" << i << " , " << j << "): " << histMatrix.at<int>(i, j) << "\t\t";
	//	}
	//	std::cout << "\n";
	//}

	return 1;
}

//int ColorTransformer::DrawHistogram(const Mat& histMatrix, Mat& histImage) {
//	if (histMatrix.data == NULL)
//		return 0;
//
//	int histSize = 256;
//	int hist_w = 1024, hist_h = 768;
//	Scalar scalar[] = { Scalar(255, 0, 0) ,Scalar(0, 255, 0),Scalar(0, 0, 255) };
//	int bin_w = cvRound((double)hist_w / histSize);
//	if (histMatrix.cols == 1) {
//		histImage = cv::Mat(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
//	}
//	else {
//		histImage = cv::Mat(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
//	}
//
//	int histImagechannel = histImage.channels();
//
//	normalize(histMatrix, histMatrix, 0, histImage.cols, NORM_MINMAX, -1, Mat());
//
//	for (int i = 1; i < histSize; i++)
//	{
//		for (int j = 0; j < histImagechannel; j++) {
//			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(histMatrix.at<int>((i - 1), j))),
//				Point(bin_w*(i), hist_h - cvRound(histMatrix.at<int>((i - 1), j))),
//				scalar[j], 2, 8, 0);
//		}
//
//	}
//	imshow("calcHist Demo", histImage);
//	waitKey();
//	return EXIT_SUCCESS;
//}

int ColorTransformer::DrawHistogram(const Mat& sourceImage, Mat& destinationImage)
{
	// Thiết lập các thông số cho ảnh histogram
	int histSize = 256;
	int hist_w = 0;
	int hist_h = 400;
	int bin_w = 2;

	if (sourceImage.channels() == 1) {
		hist_w = histSize * bin_w;
	}
	else if (sourceImage.channels() == 3) {
		hist_w = histSize * bin_w * 3;
	}

	Mat tempHistImg(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	Mat calculatedHist;

	// Tính phân bố histogram
	CalcHistogram(sourceImage, calculatedHist);

	if (sourceImage.channels() == 1)
	{

		// Chuẩn hóa để histogram vừa vặn với ảnh
		//normalize(calculatedHist, calculatedHist, 0, tempHistImg.rows, NORM_MINMAX, -1, Mat());

		// Vẽ histogram lên ảnh

		for (int i = 0; i < histSize; i++) {
			line(tempHistImg, Point(bin_w * (i), hist_h),
				Point(bin_w * (i), hist_h - cvRound(calculatedHist.at<float>(i))),
				Scalar(220, 220, 220), bin_w, 8, 0);
		}
	}
	else if (sourceImage.channels() == 3)
	{
		std::vector<Mat> bgr_planes;
		split(calculatedHist, bgr_planes);


		normalize(bgr_planes[0], bgr_planes[0], 0, tempHistImg.rows, NORM_MINMAX, -1, Mat());
		normalize(bgr_planes[1], bgr_planes[1], 0, tempHistImg.rows, NORM_MINMAX, -1, Mat());
		normalize(bgr_planes[2], bgr_planes[2], 0, tempHistImg.rows, NORM_MINMAX, -1, Mat());

		//Vẽ histogram của từng kênh màu lên ảnh
		for (int i = 0; i < histSize; i++)
		{
			line(tempHistImg, Point(bin_w * (i), hist_h),
				Point(bin_w * (i), hist_h - cvRound(bgr_planes[0].at<float>(i))),
				Scalar(255, 0, 0), bin_w, 8, 0);
		}

		for (int i = 0; i < histSize; i++) {
			line(tempHistImg, Point(bin_w * (i + histSize), hist_h),
				Point(bin_w * (i + histSize), hist_h - cvRound(bgr_planes[1].at<float>(i))),
				Scalar(0, 255, 0), bin_w, 8, 0);
		}

		for (int i = 0; i < histSize; i++) {
			line(tempHistImg, Point(bin_w * (i + histSize * 2), hist_h),
				Point(bin_w * (i + histSize * 2), hist_h - cvRound(bgr_planes[2].at<float>(i))),
				Scalar(0, 0, 255), bin_w, 8, 0);
		}

	}
	tempHistImg.copyTo(destinationImage);
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {
	if (sourceImage.data == NULL)
		return 0;
	int width = sourceImage.cols, height = sourceImage.rows;
	int srcChannels = sourceImage.channels();

	if (srcChannels == 1) {
		destinationImage = cv::Mat(height, width, CV_8UC1);
	}
	else {
		destinationImage = cv::Mat(height, width, CV_8UC3);
	}

	int dstChannels = destinationImage.channels();
	std::cout << "So kenh mau: " << dstChannels << "\n";
	int srcWidthStep = sourceImage.step[0];
	int dstWidthStep = destinationImage.step[0];

	std::cout << srcWidthStep << "\n";
	//3*853 = 2559
	std::cout << dstWidthStep;

	uchar* pSrcData = sourceImage.data;
	uchar* pDstData = destinationImage.data;

	for (int y = 0; y < height; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep) {
		uchar* pSrcRow = pSrcData;
		uchar* pDstRow = pDstData;
		for (int x = 0; x < width; x++, pSrcRow += srcChannels, pDstRow += dstChannels) {
			for (int i = 0; i < srcChannels; i++) {
				// saturate_cast<uchar> lam tron pixel ve [0..255] dung nhat vd 257->1 (sai) 
				// => 257->255 (dung) 
				pDstRow[i] = saturate_cast<uchar>(pSrcRow[i] * c);
			}
		}
	}

	return 1;
}


int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.data == NULL)
		return 0;
	int srcChannels = sourceImage.channels();
	Mat hist;
	CalcHistogram(sourceImage, hist);
	
	Mat newHist = Mat::zeros(256, srcChannels, CV_32S);

	int width = sourceImage.cols, height = sourceImage.rows;

	int total = width * height;

	if (srcChannels == 1) {
		destinationImage = cv::Mat(height, width, CV_8UC1);
	}
	else {
		destinationImage = cv::Mat(height, width, CV_8UC3);
	}

	int dstChannels = destinationImage.channels();

	int srcWidthStep = sourceImage.step[0];
	int dstWidthStep = destinationImage.step[0];

	uchar* pSrcData = sourceImage.data;
	uchar* pDstData = destinationImage.data;

	int curr[3] = { 0 }; // cur 3 kenh mau

	//accumulate histogram
	for (int i = 0; i < hist.rows; i++) {
		for (int j = 0; j < hist.cols; j++)
		{
			curr[j] += hist.at<int>(i, j); //tich luy
			newHist.at<int>(i, j) = round((((float)curr[j]) * 255) / total);//chuan hoa
		}
	}

	for (int y = 0; y < height; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep) {
		uchar* pSrcRow = pSrcData;
		uchar* pDstRow = pDstData;
		for (int x = 0; x < width; x++, pSrcRow += srcChannels, pDstRow += dstChannels) {
			for (int i = 0; i < srcChannels; i++) {
				pDstRow[i] = newHist.at<int>((pSrcRow[i]), i);
			}
		}
	}


	return 1;
}

#pragma region Can bang kenh V trong hsv
//int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage) {
//	if (sourceImage.data == NULL)
//		return 0;
//
//	int srcChannels = sourceImage.channels();
//	Mat hist, hsv;
//	Converter ct;
//
//	CalcHistogram(sourceImage, hist);
//
//	Mat newHist = Mat::zeros(256, 1, CV_32S);
//
//	int width = sourceImage.cols, height = sourceImage.rows;
//
//	int total = width * height;
//
//	if (srcChannels == 1) { //grayscale
//		destinationImage = cv::Mat(height, width, CV_8UC1);
//	}
//	else { //color
//		destinationImage = cv::Mat(height, width, CV_8UC3);
//
//		Mat src;
//		sourceImage.copyTo(src);
//		ct.Convert(src, hsv, 2); // hsv mat
//		CalcHistogram(hsv, hist);// histogram of hsv
//	}
//
//
//	int dstChannels = destinationImage.channels();
//
//	int srcWidthStep = sourceImage.step[0];
//	int dstWidthStep = destinationImage.step[0];
//
//	uchar* pSrcData = sourceImage.data;
//	uchar* pDstData = destinationImage.data;
//
//	//accumulate histogram va chuan hoa
//	std::cout << srcChannels << std::endl;;
//	if (srcChannels == 1) {
//		int curr = 0;
//		for (int i = 0; i < hist.rows; i++) {
//			curr += hist.at<int>(i, 0); // tich luy
//			newHist.at<int>(i, 0) = round((((float)curr) * 255) / total);//chuan hoa
//		}
//
//		for (int y = 0; y < height; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep) {
//			uchar* pSrcRow = pSrcData;
//			uchar* pDstRow = pDstData;
//
//			for (int x = 0; x < width; x++, pSrcRow += srcChannels, pDstRow += dstChannels) {
//				for (int i = 0; i < srcChannels; i++) {
//					pDstRow[i] = newHist.at<int>((pSrcRow[i]), i);
//
//				}
//			}
//		}
//	}
//	else {
//		int curr = 0;
//		for (int i = 0; i < hist.rows; i++) {
//			curr += hsv.at<int>(i, 0);
//			newHist.at<int>(i, 2) = round((((float)curr) * 255) / total);  //chuan hoa kenh V trong hsv
//
//			Mat temp;
//			newHist.copyTo(temp);
//			ct.Convert(temp, newHist, 3);
//		}
//
//		for (int y = 0; y < height; y++, pSrcData += srcWidthStep, pDstData += dstWidthStep) {
//			uchar* pSrcRow = pSrcData;
//			uchar* pDstRow = pDstData;
//
//			for (int x = 0; x < width; x++, pSrcRow += srcChannels, pDstRow += dstChannels) {
//				for (int i = 0; i < srcChannels; i++) {
//					pDstRow[i] = newHist.at<int>((pSrcRow[i]), i);
//
//				}
//			}
//		}
//	}
//
//	return 1;
//}

#pragma endregion

float dist(float a[], float b[]) {
	float* euc = new float[256];
	float lib1 = 0;
	float lib2 = 0;
	for (int i = 0; i < 256; i++)
	{
		lib1 += powf(a[i], 2);
		lib2 += powf(b[i], 2);
	}
	lib1 = sqrt(lib1);
	lib2 = sqrt(lib2);
	printf("%f	%f\n", lib1, lib2);

	for (int i = 0; i < 256; i++)
	{
		a[i] = a[i] / lib1;
		b[i] = b[i] / lib2;
	}
	for (int i = 0; i < 256; i++)
	{
		euc[i] = sqrtf(powf((a[i] - b[i]), 2));
	}
	for (int i = 0; i < 256; i++)
		printf("%f\n", euc[i]);
	return 1;

}
float ColorTransformer::CompareImage(const Mat& image1, Mat& image2) {
	ColorTransformer CV;
	Mat max1;
	Mat max2;
	CV.CalcHistogram(image1, max1);
	CV.CalcHistogram(image2, max2);
	float res = 0;
	float* a = new float[max1.rows];
	float* b = new float[max2.rows];
	for (int i = 0; i < max1.rows; i++) {
		a[i] = 0;
		b[i] = 0;
	}
	for (int i = 0; i < max1.rows; i++) {
		for (int j = 0; j < max1.cols; j++) {
			a[i] += max1.at<int>(i, j);
		}
		a[i] /= max1.cols;
	}
	for (int i = 0; i < max2.rows; i++) {
		for (int j = 0; j < max2.cols; j++) {
			b[i] += max2.at<int>(i, j);
		}
		b[i] /= max2.cols;
	}
	dist(a, b);
	printf("\n\n\n\n\n\n");
	for (int i = 0; i < max2.rows; i++) {
		for (int j = 0; j < max2.cols; j++) {
			printf("%f\n", res);
			res += abs(a[i] - b[i]);
		}
	}
	res /= 256;
	delete[] a;
	delete[] b;
	printf("%f", res);
	return res;
}

