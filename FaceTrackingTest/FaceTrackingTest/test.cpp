#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <cmath>

using namespace std;
using namespace cv;

CascadeClassifier faceCascade, eyeCascade;
String windowName = "Face Tracking";
String faceWinName = "Face View";
int x_center, y_center;

void faceTrack(Mat frame) {
	vector<Rect> faces;
	Mat frameGray;

	cvtColor(frame, frameGray, COLOR_BGR2GRAY);
	equalizeHist(frameGray, frameGray);

	faceCascade.detectMultiScale(frameGray, faces, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++) {
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		//cout << center << endl;
		x_center = faces[i].x + faces[i].width / 2;
		y_center = faces[i].y + faces[i].height / 2;
		cout << x_center << ", " << y_center << endl;
		Mat face = frameGray(faces[i]);
		vector<Rect> eyes;

		eyeCascade.detectMultiScale(face, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

		if (eyes.size() > 0) {
			ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		}
	}

	imshow(windowName, frame);
}

int main() {
	VideoCapture capture(0);
	Mat frame;
	Point prevCenter(0, 0);

	faceCascade.load("haarcascade_frontalface_alt.xml");
	eyeCascade.load("haarcascade_eye.xml");

	while (capture.read(frame)) {
		faceTrack(frame);

		if (waitKey(30) >= 0) {
			break;
		}
	}

	return 0;
}
