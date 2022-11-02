//  Developer: Baturay Özgürün
//  Copyright © 2018 Baturay Özgürün
//  If you have any requests or questions please do not hesitate to contact me via baturay.ozgurun@gmail.com

//  Project Name: Computer Vision Project -- Object Recognition Algorithm with OpenCV
//  Project Desciption: This project aims to perform an object recognition technique capitalizing on the Elliptic Fourier Descriptors method. A small training dataset (coin images) is utilized to find the mean descriptor that requires segmentation operations. Object recognition is performed by calculating the Euclidean distance between the mean descriptor and the descriptor of the test image. This calculation operation allows us to evaluate object similarities. This code also provides a similarity matrix (evaluation of the object recognition task) in a text file.

//  This code works as follows:
//  1. Read the image
//  2. Make the given image as a binary image
//  3. Segment the binary image
//  4. Make small segments as background segment
//  5. Convert segmented image into gray scale image
//  6. Use the gray scale image for the Elliptic Fourier Descriptor Calculation
//  7. In the EFD function, convert the image into binary image to find edges
//  8. Find the contours of the edged image
//  9. Approximate contours to polygons to find original contours
//  10. Find moments on the calculated contours regions.
//  11. Calculate the mass center of the moments.
//  12. Calculate the Elliptic Fourier Descriptors (EFD) for the contours whose size is bigger than 100.
//  13. Assign each EFD coefficients (a,b,c and d) to the separate matrices (AA, BB, CC, DD)
//  14. Sort the variables in these matrices seperateley.
//  15. Divide the biggest and smallest EFD coefficients separately, and assign these coeff to the a matrix named EFDes.
//  16. Get the EFDes values for each training object (e.g. coin in this code), and take the average of the descriptors
//  17. To examine the similarity of the given objects, Euclidean Distance is calculated between EFDes values for the examined and training objects
//  18. After this calculation, similarity of the objects are evaluated, and their results are written on the console and a text file

// Please note that the "ellipticFourierDescriptors()" function was obtained from the Intelligence Systems Laboratory at Bogazici University.

//Include headers
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv/highgui.h>
const float pi = 3.14;

//Include Namespaces
using namespace cv;
using namespace std;

//Define Functions
double** allocate2D(int, int);
Mat ellipticFourierDescriptors(Mat,int);
Mat Segmentation(Mat);

int main (){
    //Define variables
    Mat Image, SegmentedImage, GrayImage;
    Mat AvrDes=Mat::zeros(4,1,cv::DataType<double>::type);
    Mat CalculatedDescriptor=Mat::zeros(4,1,cv::DataType<double>::type);
    Mat Dif=Mat::zeros(4,1,cv::DataType<double>::type);
    double L2Norm=0;
    
    //Constructing Library for a Training Object (e.g. Coin)
    for(int i=1;i<=5;i++){
        Mat EFDes; //Define descriptor variable
        ostringstream name, file_name; //To read image sequentially, we need to define a string for a training image
        name << "data/coin" << i << ".jpg"; //Assign a name for the training image
        Image = imread(name.str(),CV_LOAD_IMAGE_GRAYSCALE); //Read the image
        
        SegmentedImage = Segmentation(Image); //Segment the image using the constructed function
        cvtColor(SegmentedImage,GrayImage,CV_BGR2GRAY); //Convert the image into gray scale image
        EFDes=ellipticFourierDescriptors(GrayImage,8); //Execute efd.cpp file
        AvrDes=AvrDes+EFDes; //Sum the all descriptors
        file_name << "Segmented Coin" << i << ".jpg";
        imshow(file_name.str(),SegmentedImage); //Show the segmented coin images
        imwrite("results/" + file_name.str() + ".jpg", SegmentedImage); //Write the segmented coin images
        waitKey(1000);
    }
    AvrDes = AvrDes/5; //Average the descriptors
    
    cout << "The size of the Elliptic Fourier Descriptors is different for each object (even same objects like coins).";
    cout << "Instead of substacting EFD values from each other, the biggest and smallest descriptors are divided for ";
    cout << "each a-b-c-d coeffs separately.";
    cout << "In this way, the length of the obtained descriptors is four." << endl << endl;
    cout << "Mean Desciptor is calculated as:  " << endl << AvrDes << endl << endl;
    
    ofstream outputfile;
    outputfile.open("results/Similarity Matrix.txt");
    
    //Distinguishing Objects
    for(int i=1;i<=2;i++){
        ostringstream object, test_file_name; //To read image sequentially, we need to define a string for object recognition
        object << "data/test" << i << ".jpg"; //Assign a name for the images
        Image = imread(object.str(),CV_LOAD_IMAGE_GRAYSCALE); //Read the image
    
        SegmentedImage=Segmentation(Image); //Segment the image using the constructed function
        cvtColor(SegmentedImage,GrayImage,CV_BGR2GRAY); //Convert the image into gray scale imag
        CalculatedDescriptor=ellipticFourierDescriptors(GrayImage,8); //Execute efd.cpp file
        Dif=AvrDes-CalculatedDescriptor; //Find the descriptor differences between the training and examined object desciptors
        
        test_file_name << "Segmented Test Images" << i << ".jpg";
        imshow(test_file_name.str(),SegmentedImage); //Show the segmented test images
        imwrite("results/" + test_file_name.str() + ".jpg", SegmentedImage); //Write the segmented test images
        waitKey(1000);
        
        for(int i=0;i<4;i++){
            L2Norm=L2Norm+Dif.at<double>(i)*Dif.at<double>(i); //Find the L2 Norm or Euclidean Distance
        }
    
        //It is observed that when the L2Norm is bigger than 200, the examined object is either very different or similar with substantially changed object than the training object (e.g. coin)
        
        //Write the similary of the exemined object on not only the comsole but also the text file named Similarity Matrix.txt
        if(L2Norm<=25){
            outputfile << "The " << i << "th test object is identical as the images in the dataset" << endl;
            cout << "The " << i << "th test object is identical as the images in the dataset" << endl;
        }
        else{
            outputfile << "The " << i << "th test object is (slightly) different from the dataset images" << endl;
            cout << "The " << i << "th test object is (slightly) different from the dataset images" << endl;
        }
    }
    return 0;
}

Mat Segmentation(Mat Image){
    Mat SegmentedImage,BinaryImage, SegmentedImageAppearance; //Define Mat variables
    int Ns; //Define integer variable for the total number of segments
    
    threshold(Image, BinaryImage, 150, 255, THRESH_BINARY); //Convert the grayscale image to binary
    //connectedComponents calculates segments of the given binary image (BinaryImage) with 8  connectedness
    Ns = connectedComponents(BinaryImage, SegmentedImage, 8, CV_32S); //It also counts total number of segments (Ns)
    
    normalize(SegmentedImage,SegmentedImageAppearance,0,255,NORM_MINMAX,CV_8U); //Seperate calculated segments in 8 bit scale
    applyColorMap(SegmentedImageAppearance,SegmentedImageAppearance,COLORMAP_JET); //Make colorful representation of the segmented image.
    
    
    Mat list = Mat::zeros(Ns,1,CV_32SC1); //Create list variable to keep the number of segments
    for (int k=0;k<Ns;k++){
        int tut=0;
        for (int j=0; j<SegmentedImage.cols; j++){
            for (int i=0; i<SegmentedImage.rows;i++){
                if(SegmentedImage.at<int>(i,j)==k){
                    tut=tut+1;
                }
            }
        }
        list.at<int>(k,0)=tut; //Write the number of segments. Row number indicates the segment numbers.
    }
    
    
    //Apply threshold to the segments to remove from the image using segment numbers (kept in list variable)
    int t=round(0.0001*SegmentedImage.cols*SegmentedImage.rows); //Threshold Value
    for (int k=0;k<Ns;k++){
        if(list.at<int>(k,0)<t){ //When the value in the list is less than the threshold
            for (int j=0; j<SegmentedImage.cols; j++){ //Scan across column
                for (int i=0; i<SegmentedImage.rows;i++){ //Scan across row
                    if(SegmentedImage.at<int>(i,j)==k){ //When the pixel is equal to the conditioned k value
                        SegmentedImage.at<int>(i,j)=1; //Make that pixel as background
                    }
                }
            }
        }
    }
    
    normalize(SegmentedImage,SegmentedImageAppearance,0,255,NORM_MINMAX,CV_8U); //Seperate calculated segments in 8 bit scale
    applyColorMap(SegmentedImageAppearance,SegmentedImageAppearance,COLORMAP_JET); //Make colorful representation of the segmented image
    return SegmentedImageAppearance;
}


//There Is No Segmentation Operation In The EFD. It Finds The Edges Of The Segmented Image
Mat ellipticFourierDescriptors(Mat src,int harmonics)
{
    //namedWindow("src", CV_WINDOW_AUTOSIZE);
    //imshow("src", src);
    
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    
    Mat drawing1 = Mat::zeros(src.size(), CV_8UC1);
    Mat drawing2 = Mat::zeros(src.size(), CV_8UC1);
    
    // Detect Edges Using Threshold Method
    threshold(src, src, 35, 255, THRESH_BINARY);
    //imshow("src1", src);
    
    // Find Contours
    findContours(src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    
    // Approximate Contours to Polygons
    vector<vector<Point> > contours_poly1(contours.size());
    vector<vector<Point> > contours_poly2(contours.size());
    
    for (int i=0; i<contours.size();i++){
        approxPolyDP(Mat(contours[i]), contours_poly1[i], 3, true);
        drawContours(drawing1,contours_poly1,i,255,1,8);
    }
    
    // Original contours are shown for comparison purposes
    //imshow("Original Contours", drawing1);
    //imwrite("Original Contours.jpg", drawing1);
    
    // Moments are found. It calculates the contours regions and finds how many regions then calculates for moments.
    vector<Moments> mu(contours.size());
    for (int i=0;i<contours.size();i++){
        mu[i] = moments(contours[i],false);
    }
    
    //  The mass center of each contour is found
    vector<Point2f> a0(contours.size());
    for (int i = 0; i < contours.size(); i++){
        a0[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
    }
    
    // The period and the number of harmonics are specified
    int P = harmonics;
    double T, **a, **b, **c, **d;
    
    //2D arrays are allocated for a,b,c,d variables of the elliptic Fourier descriptors analysis
    double csize=contours.size();
    a = allocate2D(csize,P+1);
    b = allocate2D(csize,P+1);
    c = allocate2D(csize,P+1);
    d = allocate2D(csize,P+1);
    
    Mat AA,BB,CC,DD;
    
    double w, dxt, dyt;
    int counter = 0, DesC=0;
    double threshold = 0;
    //double tut=0, tut1=0;
    
    //Find the biggest contour on the object
    for (int i=0;i<contours.size();i++){
        if (contours[i].size()>threshold){
            threshold=contours[i].size();
        }
    }
    
    for (int i=0;i<contours.size();i++){
        // If the size of the contour is below the threshold, the elliptic Fourier descriptors are not found for this tiny contour
        if (contours[i].size()>=threshold){
            T = contours[i].size();
            //Assign EFD coefficients such as a, b, c and d to the AA, BB, CC and DD matrices
            AA=Mat::zeros(T*P,1,cv::DataType<double>::type);
            BB=Mat::zeros(T*P,1,cv::DataType<double>::type);
            CC=Mat::zeros(T*P,1,cv::DataType<double>::type);
            DD=Mat::zeros(T*P,1,cv::DataType<double>::type);
            
            w = 2 * pi / T;
            for (int tp=1;tp<contours[i].size();tp++){
                for (int k=1;k<= P;k++){
                    dxt = contours[i][tp].x - contours[i][tp - 1].x;
                    dyt = contours[i][tp].y - contours[i][tp - 1].y;
                    a[counter][k] += (1 / (pi*w*k*k)) * dxt * (cos(k*w*tp) - cos(k*w*(tp - 1)));
                    b[counter][k] += (1 / (pi*w*k*k)) * dxt * (sin(k*w*tp) - sin(k*w*(tp - 1)));
                    c[counter][k] += (1 / (pi*w*k*k)) * dyt * (cos(k*w*tp) - cos(k*w*(tp - 1)));
                    d[counter][k] += (1 / (pi*w*k*k)) * dyt * (sin(k*w*tp) - sin(k*w*(tp - 1)));
                    
                    AA.at<double>(DesC)=a[counter][k];
                    BB.at<double>(DesC)=b[counter][k];
                    CC.at<double>(DesC)=c[counter][k];
                    DD.at<double>(DesC)=d[counter][k];
                    DesC++;
                }
            }
            counter++;
        }
    }
    
    //Define variable for the descriptors
    Mat AADes,BBDes,CCDes,DDDes;
    Mat EFDes=Mat::zeros(4,1,cv::DataType<double>::type);
    
    //Sort EFD coefficents
    cv::sort(AA,AADes,CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    cv::sort(BB,BBDes,CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    cv::sort(CC,CCDes,CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    cv::sort(DD,DDDes,CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    
    //Divide the bigest and the smallest EFD coefficients separately, and assign calculated coeff variables to the EFDes matrix
    EFDes.at<double>(0)=AADes.at<double>(0)/AADes.at<double>(AADes.rows-1);
    EFDes.at<double>(1)=BBDes.at<double>(0)/BBDes.at<double>(BBDes.rows-1);
    EFDes.at<double>(2)=CCDes.at<double>(0)/CCDes.at<double>(CCDes.rows-1);
    EFDes.at<double>(3)=DDDes.at<double>(0)/DDDes.at<double>(DDDes.rows-1);

    // Once all the needed variables are found, reconstruction starts from here
    counter = 0;
    for (int i = 0; i < contours.size(); i++){
        if (contours[i].size()>=threshold){
            a[i][0]=(double)a0[i].x;
            b[i][0]=(double)a0[i].y;
            T = contours[i].size();
            w=2*pi/T;
            
            for (int tp=1;tp<contours[i].size();tp++){
                contours[i][tp].x = a0[i].x;
                contours[i][tp].y = a0[i].y;
            }
            for (int tp=1;tp<contours[i].size();tp++){
                for (int k = 1; k <= P; k++){
                    contours[i][tp].x += a[counter][k] * cos(k*w*tp) + b[counter][k] * sin(k*w*tp);
                    contours[i][tp].y += c[counter][k] * cos(k*w*tp) + d[counter][k] * sin(k*w*tp);
                }
            }
            counter++;
        }
    }
    
    for (int i=0;i<contours.size();i++){
        if (contours[i].size()>=threshold){
            approxPolyDP(Mat(contours[i]), contours_poly2[i], 3, true);
            drawContours(drawing2, contours_poly2, i, 255, 1, 8, vector<Vec4i>(), 0, Point());
        }
    }
    //imshow("New Contours", drawing2);
    //imwrite("New Contours.jpg", drawing2);
    return EFDes; //Return the calculated length-4 descriptor sequence
}

//2D array is allocated using this function
double** allocate2D(int n, int m){
    double **array2D;
    array2D = (double**)calloc(n, sizeof(double *));
    for (size_t i = 0; i < n; i++){
        array2D[i] = (double*)calloc(m, sizeof(double));
    }
    return array2D;
}

