#include "image.h"
#include <lodepng.h>
#include <stdio.h>
#include <string.h>

#define SAVE_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

color3 *getPixelPtr(Image *img, size_t x, size_t y) {
    return &(img->data[y * img->width + x]);
}

Image *initImage(size_t width, size_t height) {
    Image *img = (Image*) malloc(sizeof(Image));
    img->width = width;
    img->height = height;
    img->data = (color3 *)malloc(sizeof(color3)*width*height);
    return img;
}

void freeImage(Image *img) {
    free(img->data);
    free(img);
}

void saveImage(Image *img, char *basename) {
#ifdef SAVE_PNG
  char filename[256+4];
  strcpy(filename, basename);
  strcat(filename, ".png");

  size_t cpt = 0;
  unsigned char *image = new unsigned char [img->width*img->height*3];
  // write image to file
  for(unsigned y = 0; y < img->height; y++) {
    color3 *ptr = getPixelPtr(img, 0, img->height-y-1);
    for(unsigned x = 0; x < img->width; x++) {
      ivec3 c = clamp(ivec3(255.f**ptr), 0, 255);
      image[cpt++] = c.x;
      image[cpt++] = c.y;
      image[cpt++] = c.z;
      ++ptr;
    }
  }

  /*Encode the image*/
  unsigned error = lodepng_encode24_file(filename, image, img->width, img->height);
  delete [] image;
  /*if there's an error, display it*/
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
#else
    // save the image to basename.ppm
    {
        FILE *fp = NULL;
        char filename[256+4];
        strcpy(filename, basename);
        strcat(filename, ".ppm");

        fp = fopen(filename, "w");

        if (fp) {
            // file is created

            fprintf(fp, "P3\n");
            fprintf(fp, "%zu %zu\n255\n", img->width, img->height);

            // write image to file
            for(unsigned y = 0; y < img->height; y++) {
                color3 *ptr = getPixelPtr(img, 0, img->height-y-1);
                for(unsigned x = 0; x < img->width; x++) {
                    ivec3 c = clamp(ivec3(255.f**ptr), 0, 255);
                    unsigned char r = c.x;
                    unsigned char g = c.y;
                    unsigned char b = c.z;
                    fprintf(fp, "%d %d %d  ", r, g, b);
                    ++ptr;
                }
                fprintf(fp, "\n");
            }
            fclose(fp);
        }
    }
#endif

}

//Function to get an Image from a file
Image *loadImagePNG(char *filename){
	unsigned error;
	unsigned char* data;
	unsigned width = 0, height = 0;

	if(!error) error = lodepng_decode32_file(&data, &width, &height, filename);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

	Image *image = initImage(size_t(width), size_t(height));

	int j = 0;
	for (int i = 0 ; i < width*height ; ++i){
		image->data[i].r = float(data[j++])/255.f;
        image->data[i].g = float(data[j++])/255.f;
        image->data[i].b = float(data[j++])/255.f;
	}

	return image;
}

Image * loadImagePPM(char *filename){
	int i, width,height,max_value;
	char format[8];
	Image * image;
	FILE * f = fopen(filename,"r");
	if (!f){
		fprintf(stderr,"Cannot open file %s...\n",filename);
		exit(-1);
	}
	fscanf(f,"%s\n",format);
	assert( (format[0]=='P' && format[1]=='3'));  // check P3 format
	while(fgetc(f)=='#') // commentaire
	{
		while(fgetc(f) != '\n'); // aller jusqu'a la fin de la ligne
	}
	fseek( f, -1, SEEK_CUR);
	fscanf(f,"%d %d\n", &width, &height);
	fscanf(f,"%d\n", &max_value);
	image = initImage(width, height);
	assert(image != NULL);
	assert(image->data != NULL);

	for(i=0 ; i<width*height ; i++){
		int r,g,b;
		fscanf(f,"%d %d %d", &r, &g, &b);
		image->data[i].r = float(r)/255.f;
		image->data[i].g = float(g)/255.f;
		image->data[i].b = float(b)/255.f;
	}
	fclose(f);
	return image;
}

Image * loadImageJPG(char *filename){
	int width, height, bpp;

	uint8_t* rgb_image = stbi_load(filename, &width, &height, &bpp, 3);

	Image *img = initImage(unsigned(width), unsigned(height));

	int j=0;
	for (unsigned i = 0 ; i < width*height ; ++i){
		img->data[i].r = float(rgb_image[j++])/255.f;
		img->data[i].g = float(rgb_image[j++])/255.f;
		img->data[i].b = float(rgb_image[j++])/255.f;
	}

	stbi_image_free(rgb_image);

	return img;
}