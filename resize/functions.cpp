#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include "functions.h"


//   http://paulcuth.me.uk/netpbm-viewer/
using namespace std;

Pixel** createImage(int width, int height) {
  cout << "Start createImage... " << endl;
  
  // Create a one dimensional array on the heap of pointers to Pixels 
  //    that has width elements (i.e. the number of columns)
  Pixel** image = new Pixel*[width];
  
  bool fail = false;
  
  for (int i=0; i < width; ++i) { // loop through each column
    // assign that column to a one dimensional array on the heap of Pixels
    //  that has height elements (i.e. the number of rows)
    image[i] = new Pixel[height];
    
    if (image[i] == nullptr) { // failed to allocate
      fail = true;
    }
  }
  
  if (fail) { // if any allocation fails, clean up and avoid memory leak
    // deallocate any arrays created in for loop
    for (int i=0; i < width; ++i) {
      delete [] image[i]; // deleting nullptr is not a problem
    }
    delete [] image; // dlete array of pointers
    return nullptr;
  }
  
  // initialize cells
  //cout << "Initializing cells..." << endl;
  for (int row=0; row<height; ++row) {
    for (int col=0; col<width; ++col) {
      //cout << "(" << col << ", " << row << ")" << endl;
      image[col][row] = { 0, 0, 0 };
    }
  }
  cout << "End createImage... " << endl;
  return image;
}

void deleteImage(Pixel** image, int width) {
  cout << "Start deleteImage..." << endl;
  // avoid memory leak by deleting the array
  for (int i=0; i<width; ++i) {
    delete [] image[i]; // delete each individual array placed on the heap
  }
  delete [] image;
  image = nullptr;
}

int* createSeam(int length) {
	int* seam = new int[length];
	for (int i = 0; i < length; i++){
		seam[i] = 0;
	}
  return seam;
}

void deleteSeam(int* seam) {
	delete[] seam;
}

bool loadImage(string filename, Pixel** image, int width, int height) {
	ifstream infile;
	infile.open(filename);
	if (!infile.is_open()){
		cout << "Error: failed to open input file - " << filename << endl;
		return false;
	}
	char type[3];
    infile >> type; // should be P3
    if ((toupper(type[0]) != 'P') || (type[1] != '3')) { // check that type is correct
       cout << "Error: type is " << type << " instead of P3" << endl;
       return false;
    }
	
  
  // get width (w) and height (h) from preamble
    int w = 0, h = 0;
    infile >> w >> h;
	if (infile.fail()){
		 cout << "Error: read non-integer value" << endl;
		 return false;
	}
    if (w != width) { // check that width matches what was passed into the function
       cout << "Error: input width " << "(" << width << ") does not match value in file ("
       << w << ")" << endl;
       return false;
     }
    if (h != height) { // check that height matches what was passed into the function
       cout << "Error: input height " << "(" << height << ") does not match value in file ("
       << h << ")" << endl;
       return false;
     }
	
  
      // get maximum value from preamble
     int colorMax = 0;
     infile >> colorMax;
     if (colorMax != 255){
        cout << "Error: file is not using RGB color values." << endl;
        return false;
     }
	 if (infile.fail()){
		 cout << "Error: read non-integer value" << endl;
		 return false;
	 }
	
    // get RGB pixel values
    // complete this part
      for (int r = 0; r < height; r++){
	     for (int c = 0; c < width; c++){
		     infile >> image[c][r].r;
			 if (image[c][r].r < 0 || image[c][r].r > 255){
				 cout << "Error: invalid color value " << image[c][r].r << endl;
				 return false;
			 }
			 if (infile.fail()){
				 if (infile.eof()){
					cout << "Error: not enough color values" << endl;}
				 else {
					 cout << "Error: read non-integer value" << endl;
				 }
				 return false;
			 }
		     infile >> image[c][r].g;
			  if (image[c][r].g < 0 || image[c][r].g > 255){
				  cout << "Error: invalid color value " << image[c][r].g << endl;
				 return false;
			 }
			  if (infile.fail()){
				 if (infile.eof()){
					cout << "Error: not enough color values" << endl;}
				 else {
					 cout << "Error: read non-integer value" << endl;
				 }
				 return false;
			 }
		     infile >> image[c][r].b;
			  if (image[c][r].b < 0 || image[c][r].b > 255){
				  cout << "Error: invalid color value " << image[c][r].b << endl;
				 return false;
			 }
			 if (infile.fail()){
				 if (infile.eof()){
					cout << "Error: not enough color values" << endl;}
				 else {
					 cout << "Error: read non-integer value" << endl;
				 }
				 return false;
			 }
	    }
       }
	   int next;
	   infile >> next;
	   if (infile.good()){
		   cout << "Error: too many color values" << endl;
		   return false;
	    }
  return true;
}

bool outputImage(string filename, Pixel** image, int width, int height) {
	ofstream output;
	output.open(filename);
	if (!output.is_open()){
		cout << "Error: failed to open output file " << filename << endl;
		return false;
	}
	output << "P3" << endl;
	output << width << ' ' << height << endl;
	output << 255 << endl;
	
	for (int r = 0; r < height; r++){
		for (int c = 0; c < width; c++){
			output << image[c][r].r << ' ';
			output << image[c][r].g << ' ';
			output << image[c][r].b << ' ';
		}
	}
  return true;
}

int energy(Pixel** image, int x, int y, int width, int height) {
	int x_energy;
	if (x != 0 && x != width - 1){
		x_energy = pow((image[x-1][y].r - image[x+1][y].r),2) + 
				   pow((image[x-1][y].g - image[x+1][y].g),2) +
				   pow((image[x-1][y].b - image[x+1][y].b),2);
	}
	else {
		if (x == 0){
			x_energy = pow((image[x+1][y].r - image[width-1][y].r),2)+
					   pow((image[x+1][y].g - image[width-1][y].g),2) +
				       pow((image[x+1][y].b - image[width-1][y].b),2);
		}
		else if (x == width-1){
			x_energy = pow((image[x-1][y].r - image[0][y].r),2)+
					   pow((image[x-1][y].g - image[0][y].g),2) +
				       pow((image[x-1][y].b - image[0][y].b),2);
		}
	}
	
	int y_energy;
	if (y != 0 && y != height - 1){
		y_energy = pow((image[x][y-1].r - image[x][y+1].r),2) + 
				   pow((image[x][y-1].g - image[x][y+1].g),2) +
				   pow((image[x][y-1].b - image[x][y+1].b),2);
	}
	else {
		if (y == 0){
			y_energy = pow((image[x][y+1].r - image[x][height-1].r),2)+
					   pow((image[x][y+1].g - image[x][height-1].g),2) +
				       pow((image[x][y+1].b - image[x][height-1].b),2);
		}
		else if (y == height-1){
			y_energy = pow((image[x][0].r - image[x][y-1].r),2)+
					   pow((image[x][0].g - image[x][y-1].g),2) +
				       pow((image[x][0].b - image[x][y-1].b),2);
		}
	}
	int sum_energy = x_energy + y_energy;
	
  return sum_energy;
}


int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam) {
	seam[0] = start_col;
	int total_energy = energy(image, start_col, 0, width, height);
	int forward = start_col;
	int right;
	int left;
	
	int forward_energy, left_energy, right_energy;
	
	for (int r = 1; r < height; r++){
		right = (forward + (width-1))%width;
	    left = (forward + (width+1))%width;
	
		if (left > width-1){
			left_energy = 5000000;
		}
		else{
			left_energy = energy(image, left, r, width, height);
		}
		if (right < 0){
			right_energy = 5000000;
		}
		else{
			right_energy = energy(image, right, r, width, height);
		}
		
		forward_energy = energy(image, forward, r, width, height);
		
		if (forward_energy <= left_energy && forward_energy <= right_energy){
			total_energy += forward_energy;
		}
		
		else if (left_energy <= right_energy){
			total_energy += left_energy;
			forward = left;
		}
		
		else{
			total_energy += right_energy;
			forward = right;
		}
		
		seam[r] = forward;
	}
  return total_energy;
}

int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam) {
	seam[0] = start_row;
	int total_energy = energy(image, 0, start_row, width, height);
	int forward = start_row;
	
	int forward_energy, left_energy, right_energy;
	
	int right, left;
	for (int c = 1; c < width; c++){
		right = (forward + (height+1))%height;
	    left = (forward + (height-1))%height;
	
		if (left < 0){
			left_energy = 5000000;
		}
		else{
			left_energy = energy(image, c, left, width, height);
		}
		if (right > height-1){
			right_energy = 5000000;
		}
		else{
			right_energy = energy(image, c, right, width, height);
		}
		
		forward_energy = energy(image, c, forward, width, height);
		
		if (forward_energy <= left_energy && forward_energy <= right_energy){
			total_energy += forward_energy;
		}
		
		else if (left_energy <= right_energy){
			total_energy += left_energy;
			forward = left;
		}
		
		else{
			total_energy += right_energy;
			forward = right;
		}
		seam[c] = forward;
	}
	
  return total_energy;
}

int* findMinVerticalSeam(Pixel** image, int width, int height) {
	int* seam = new int[height];
	int min_energy = 7000000;
	int index;
	int new_energy;
	for (int c = 0; c < width; c++ ){
		new_energy = loadVerticalSeam(image,c,width,height,seam);
		if (new_energy < min_energy){
			min_energy = new_energy;
			index = c;
		}
	}
	loadVerticalSeam(image,index,width,height,seam);
  return seam;
}

int* findMinHorizontalSeam(Pixel** image, int width, int height) {
 	int* seam = new int[width];
	int min_energy = 7000000;
	int index;
	int new_energy;
	for (int r = 0; r < height; r++ ){
		new_energy = loadHorizontalSeam(image,r,width,height,seam);
		if (new_energy < min_energy){
			min_energy = new_energy;
			index = r;
		}
	}
	loadHorizontalSeam(image,index,width,height,seam);
  return seam;
}

void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam) {
	for (int r = 0; r < height; r++){
		for (int c = verticalSeam[r]; c < width-1; c++){
			image[c][r] = image[c+1][r];
		}
	}
	delete[] verticalSeam;
}

void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam) {
	for (int c = 0; c < width; c++){
		for (int r = horizontalSeam[c]; r < height-1; r++){
			image[c][r] = image[c][r+1];
		}
	}
	delete[] horizontalSeam;
}