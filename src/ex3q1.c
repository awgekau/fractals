/* Purpose: Lab03
 * Author: Kaustubh Khilari
 * Date: October 13th, 2024
 * Persons discussed w/: -
 * References: https://www.w3schools.com/c/c_pointers.php, https://www.geeksforgeeks.org/dereference-pointer-in-c/, https://www.youtube.com/watch?v=DplxIq0mc_Y
 */

#include "ex3q1.h"

int recursiveFractalSeq(double ReZ, double ImZ, double ReC, double ImC, int maxIterations, int depth) {
    //This function helps us calculate the depth of a fractal sequence until it diverges, we calculate these by using the two base cases.

    if ((ReZ * ReZ + ImZ * ImZ) > 4.0) {//This helps us check for divergence (|z|^2 > 4) and returns current depth, I am using 4 here because I cannot use sqrt.
        return depth; 
    }

    
    if (depth >= maxIterations) {//This helps us check if max iterations reached and return it once reached
        return maxIterations;
    }
    /*
    * This helps us calculate the next values of real part of z and imaginary part of z
    * We use the formula given to us Zn+1 = Zn^2 + c
    * We used the multiplication formula given to us to obtain this, (a+ib)(c+id) = (ac - bd) + i(ad + bc)
    * but here we are multiplying (a+ib)(a+ib) so were going to obtain (a^2 - b^2) + i(2 * ab)
    */
    double newReZ = ReZ * ReZ - ImZ * ImZ + ReC;
    double newImZ = 2 * ReZ * ImZ + ImC;

    //Here we are recursing with the new values and increased depth
    return recursiveFractalSeq(newReZ, newImZ, ReC, ImC, maxIterations, depth + 1);
}

void mapMandelbrotToComplexPlane (int pixelX, int pixelY, int imgW, int imgH, double centerReal, double centerImaginary, double zoomFactor, double *Re, double *Im){
    // In this function, we have pixelX / imgW, what this does is that it helps us know the output in the range [0,1] 
    // that specifies if the pixel is towards the right or the left of the image, and here we add it to the centerReal which specifies if its towards the center or the right of the complex plane
    // where 0 corresponds to far left and 1 corresponds to far right, Similarly it also accounts for pixels in the vertical axis.

    // we subtract 0.5 from them to centralize it, this changes the range of the x axis of the complex plane to [-0.5,0.5] that are to be added to centerReal and does similarly for Y axis.
    // we multiply 3 so that the range adjusts to the width of the complex plane which here is 3 for x-axis, the updated range is [-1.5,1.5], similarly for Y axis the updated range would be [-1,1].

    //we divde 3 by the zoom factor based on how much we want to zoom in or zoom out, Lets say If we want to zoom 2X, pixelX / imgW would be multiplied by 1.5 and the range would be [-0.75,0.75]

    double planeW = 3.0;
    double planeH = 2.0;
    *Re = centerReal + (pixelX / (double)imgW - 0.5) * planeW / zoomFactor; //This helps us map to the real part which corresponds to the x-axis
    *Im = centerImaginary + (pixelY / (double)imgH - 0.5) * planeH / zoomFactor; //This helps us map to the imaginary part which corresponds to the y-axis

}

void mapJuliaToComplexPlane (int pixelX, int pixelY, int imgW, int imgH, double *Re, double *Im){
    // In this function, we again have pixelX / imgW, what this does is that it helps us know the output in the range [0,1] 
    // that specifies if the pixel is towards the right or the left of the image, where 0 corresponds to far left and 1 corresponds to far right.
    // Similarly it also accounts for pixels in the vertical axis.
    // and here we know that the center coordinates are fixed to 0,0
    // that is why we subtract 0.5 from them to centralize them, this changes the range of the x axis of the complex plane to [-0.5,0.5] and does similarly for Y axis.
    // we multiply 4 so that the range adjusts to the width of the complex plane which here is 4 for x-axis, the updated range is [-2,2], similarly for Y axis it is multiplied by 3 so the range is [-1.5,1.5].


    double planeW = 4.0;
    double planeH = 3.0;
    *Re = (pixelX/(double)imgW - 0.5) * planeW; //This helps us map to the real part which corresponds to the x-axis
    *Im = (pixelY/(double)imgH - 0.5)* planeH; //This helps us map to the imaginary part which corresponds to the y-axis

}

void depthToColor(int depth, int maxIterations, unsigned char *r, unsigned char *g, unsigned char *b){
    //This function helps us convert the depth of a fractal to color
    if (depth == maxIterations) { // here the sequence converges so the color is white
        // we use pointers r,g,b here so that instead of repeatedly creating new variables we just use the same ones but change their value everytime using their memory address.
        *r = *g = *b = 255;
    }
    else
    {
        double alpha = (double)depth/maxIterations; // here we calculate the alpha as depth/maxiterations and this gives us a value between 0 and 1 depending on how fast the point diverges.

        // we have (r,g,b) = apha(0,255,153), this results in the gradient shift from greyscale to green.

        *r = 0;
        *g = (unsigned char)(alpha * 255);
        *b = (unsigned char)(alpha * 153);

    }
}
    


int main(void){
    int fractalType, maxRecursiveDepth, imgW = 0, imgH = imageH, depth;
    double centerReal, centerImaginary, zoomFactor, ReC, ImC;

    scanf("%d", &fractalType);
    scanf("%d", &maxRecursiveDepth);
    if (fractalType != 0 && fractalType != 1){ //this checks if fractal type is invalid
        return 1;
    }

    if (maxRecursiveDepth <= 0) { //this checks if maxRecursiveDepth is more than 0
    return 1;
    }



    if (fractalType == 0){ // fractal type - Mandelbrot set, number of inputs (3)

        scanf("%lf %lf", &centerReal, &centerImaginary);
        scanf("%lf", &zoomFactor);
        if(zoomFactor <= 0){ //this checks if zoomFactor is greater than 0
            return 1;
        }
        imgW = (imgH * 3) / 2; //this sets the image width to a 3:2 aspect ratio, and we know image height is 720 px
    }

    else if (fractalType == 1){// fractal type - Julia set, number of inputs (2)
        scanf("%lf %lf", &ReC, &ImC);
        imgW = (imgH * 4) / 3; // this sets the image width to a 4:3 aspect ratio, and we know image height is 720 px

    }

    printf("P3\n");
    printf("%d %d\n", imgW, imgH); 
    printf("255\n");

    //this helps us loop through each pixel 
    for (int pixelY = 0; pixelY < imgH ; pixelY++) {
        for (int pixelX = 0; pixelX < imgW ; pixelX++) {
            double Re, Im;
            unsigned char r, g, b;
            int invertedPixelY = imgH - 1 - pixelY;  //This helps us invert the Y coordinate so that our image is not upside down

            if (fractalType == 0){//This helps us map pixels and calculate the depth of fractal in the mandelbrot set
                mapMandelbrotToComplexPlane(pixelX, invertedPixelY, imgW, imgH, centerReal, centerImaginary, zoomFactor, &Re, &Im);
                depth = recursiveFractalSeq(0.0, 0.0, Re, Im, maxRecursiveDepth, 0);
                depthToColor(depth, maxRecursiveDepth, &r, &g, &b); //we use the & sign here to get the memory address of the variables r, g and b which helps us modify their values.
                printf("%d %d %d ", r, g, b); // this helps us print pixel color for Mandelbrot set
            }
            else if (fractalType == 1){//This helps us map pixels and calculate the depth of fractal in the julia set
                mapJuliaToComplexPlane(pixelX, invertedPixelY, imgW, imgH, &Re, &Im);  
                depth = recursiveFractalSeq(Re, Im, ReC, ImC, maxRecursiveDepth, 0);
                depthToColor(depth, maxRecursiveDepth, &r, &g, &b);
                printf("%d %d %d ", r, g, b); // this helps us print pixel color for Julia set
            }

        }
    }
    
    return 0;
}