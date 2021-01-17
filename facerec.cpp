#include "facerec.h"

Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
bool model_loaded = false;
Mat pred_image;

FaceRec::FaceRec()
{
}

vector<QString> FaceRec::predictFaces(Mat pic)
{
    vector<QString> persons;

    Mat pic_mat = pic;

    string classifier = "D:\\Softs\\OpenCV\\opencv24136\\opencv\\sources\\data"
                        "\\haarcascades\\haarcascade_frontalface_alt.xml";


    CascadeClassifier face_cascade;
    if (!face_cascade.load(classifier)){
            cout << " Error loading file" << endl;
    }

    Mat sample_img = pic_mat;

    Mat graySacleFrame;

    cvtColor(sample_img, graySacleFrame, CV_BGR2GRAY);
    vector<Rect> faces;
    string Pname = "";
    //Mat faces;
    face_cascade.detectMultiScale(graySacleFrame, faces, 1.1, 3, 0, cv::Size(90, 90));

    model->load("faces.yml");
    model_loaded = true;

    map<int, string> names;
    names[1]="Alvee";
    names[2]="Nishita";
    names[3]="C. Tatum";
    names[4]="J. Alba";
    model->setLabelsInfo(names);

    model->save("faces.yml");

    if(faces.size()>=1){
        for (int i = 0; i < faces.size(); i++) {
            //Region of interest
            Rect face_i = faces[i];

            //Crop the roi from grya image
            Mat face = graySacleFrame(face_i);

            //Resizing the cropped image to suit to database image sizes
            Mat face_resized;
            cv::resize(face, face_resized, Size(img_width, img_height), 1.0, 1.0, INTER_CUBIC);

            //Recognizing what faces detected
            int pre_label = -1; double confidence = 0;
            model->predict(face_resized, pre_label, confidence);

            //Drawing green rectagle in recognize face
            rectangle(sample_img, face_i, CV_RGB(0, 255, 0), 3);

            Pname = model->getLabelInfo(pre_label);
            persons.push_back(QString::fromStdString(Pname));

            int pos_x = std::max(face_i.tl().x - 10, 0);
            int pos_y = std::max(face_i.tl().y - 10, 0);

            //Name the person who is in the image
            putText(sample_img, Pname, Point(pos_x, pos_y), FONT_HERSHEY_DUPLEX,
                    3.0, CV_RGB(0, 255, 0), 4.0, 8, false);
        }

        pred_image = sample_img;
    }
    else {
        cout << "No faces found" << endl;
    }
    return persons;
}

Mat FaceRec::predictedImage()
{
    return pred_image;
}



