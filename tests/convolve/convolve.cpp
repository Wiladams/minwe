/*
	Demonstrate using a convolution filter on an image
*/

#include "gui.h"
#include "rendercontext.h"
#include "screensnapshot.h"
#include "glsl.h"

#include <array>



/*
class ConvolutionProcessor : public ISample2D<PixelRGBA>
{
    int fDistance = 1;
    int fCurrentKernel = 6;   // Which kernel to use for the convolution filter
    float[][] kernels = {
    new float[] { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // Identity
    new float[] { 0.0f,-1.0f, 0.0f,-1.0f, 5.0f,-1.0f, 0.0f,-1.0f, 0.0f}, // Sharpen
    new float[] { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, // Blur
    new float[] { 1.0f, 2.0f, 1.0f, 2.0f, 4.0f, 2.0f, 1.0f, 2.0f, 1.0f}, // Gaussian blur
    new float[] { 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f}, // Edge enhance
    new float[] { 1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 1.0f, 1.0f, 1.0f, 1.0f},     // Edge detect
    new float[] { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,-1.0f},     // Emboss
    new float[] { 1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -2.0f,-1.0f},   // Sobell
    new float[] { -1.0f, -1.0f, -1.0f, -1.0f, 9.0f, -1.0f, -1.0f, -1.0f, -1.0f}    // Laplacian - sharpen
    };

public:
    enum ConvolutionKernel
    {
        Identity = 0,
        Sharpen,
        Blur,
        GaussianBlur,
        EdgeEnhance,
        EdgeDetect,
        Emboss,
        Sobell,
        Laplacian
    }



    //ConvolutionProcessor(GraphicsInterface gi, int width, int height, ConvolutionKernel whichKernel)
    //    :base(gi, width, height, Convolution_Frag)
    //{
    //    currentKernel = (int)whichKernel;
    //}

    int distance() { return fDistance; }

    
            protected override void  SetUniformVariables()
            {

                // Set the necessary variables in the program.  We let the program
                // know the width and height of the buffer so the kernel knows how
                // far it can go to get the neighboring pixels.
                // The "Kernel" variable is the 3x3 matrix that represents the 
                // actual convolution filter.
                ShaderProgram["Width"].Set(Width);
                ShaderProgram["Height"].Set(Height);
                ShaderProgram["Dist"].Set(Distance);
                int loc = ShaderProgram.GetUniformLocation("Kernel");
                GI.UniformMatrix3(loc, 1, false, kernels[CurrentKernel]);
            }
    

    PixelRGBA getValue(double u, double v, const PixelCoord& p) const
    {
        uniform int Width;
        uniform int Height;
        uniform int Dist;
        uniform mat3 Kernel;
        uniform sampler2D Tex0;

        void main(void)
        {
            int i, j;
            vec2 coord;
            float contrib = 0.0;
            vec4 sum = vec4(0.0);

            // 3x3 convolution matrix
            for (i = -1; i <= 1; i++)
                for (j = -1; j <= 1; j++) {
                    coord = gl_TexCoord[0].st + vec2(float(i) * (1.0 / float(Width)) * float(Dist), float(j) * (1.0 / float(Height)) * float(Dist));
                    sum += Kernel[i + 1][j + 1] * texture2D(Tex0, coord.xy);
                    contrib += Kernel[i + 1][j + 1];
                }

            gl_FragColor = sum / contrib;
        }
        ";
    }
}
*/

struct ConvolutionFilter
{
    float fData[3][3];
};

class ConvolutionPixelMap : public PixelMap
{
    PixelMap& fWrapped;
    NTSCGray fLuminance;    // For turning things gray
    
    float kernelContrib = 0;

    //float kernel[3][3]={ {-4.0f, -16.0f, -4.0f}, {-16.0f, 0.0f, -16.0f}, {-4.0f,-16.0f, -4.0f} }; // Blur
    //float kernel[3][3] = { {1.0f, 2.0f, 1.0f}, {2.0f, 4.0f, 2.0f}, {1.0f, 2.0f, 1.0f} };         // Gaussian blur
    float kernel[3][3] = { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} };     // Emboss
    //float kernel[3][3] = { {1.0f, 1.0f, 1.0f}, {1.0f, 8.0f, 1.0f}, {1.0f, 1.0f, 1.0f} };     // Edge detect
    //float kernel[3][3] = { {1.0f, 2.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {-1.0f, -2.0f, -1.0f} };   // Sobell
    //float kernel[3][3] = { {-1.0f, -1.0f, -1.0f}, {-1.0f, 16.0f, -1.0f}, {-1.0f, -1.0f, -1.0f} };   // tester
    


public:

    ConvolutionPixelMap(PixelMap& pmap)
        : PixelMap(pmap.x(), pmap.y(), pmap.width(), pmap.height()),
        fWrapped(pmap)
    {
        kernelContrib = 0.0;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                kernelContrib += kernel[i][j];

        if (abs(kernelContrib) < 0.000001)
            kernelContrib = 1;

    }

    virtual bool init(int w, int h) 
    { 
        // do nothing
        //return fWrapped.init(w, h); 
        return true;
    }

    virtual PixelRGBA* getPixelPointer(const int x, const int y) { return fWrapped.getPixelPointer(x, y); }
    virtual size_t bytesPerRow() const { return fWrapped.bytesPerRow(); }

    void set(const int x, const int y, const PixelRGBA c) { fWrapped.setPixel(x, y, c); }

    void setAllPixels(const PixelRGBA c) { fWrapped.setAllPixels(c); }

    //
    // Do convolution here at the point where we're about to get a pixel
    //
    PixelRGBA getPixel(const int x, const int y) const
    {
        // If we're at the borders, just return the pixel
        // maybe return the gray value
        if ((x == 0) || (x == width() - 1) || (y==0) || (y==height()-1))
            return fWrapped.get(x, y);

        // get the 8 neighbors around the source pixel


        // Unroll the loop, save on the fWrapped.getPixel() call
        int WWidth = fWrapped.width();
        PixelRGBA * p5 = fWrapped.getPixelPointer(x, y);
        PixelRGBA* p4 = p5 - 1;
        PixelRGBA* p6 = p5 + 1;
        PixelRGBA* p7 = p4 + WWidth;
        PixelRGBA* p8 = p5 + WWidth;
        PixelRGBA* p9 = p6 + WWidth;
        PixelRGBA* p1 = p4 - WWidth;
        PixelRGBA* p2 = p5 - WWidth;
        PixelRGBA* p3 = p6 - WWidth;

        // calculate the sum of things
        double sum = 0;

        
        sum += (double)fLuminance.toLuminance(*p1) * kernel[0][0];
        sum += (double)fLuminance.toLuminance(*p2) * kernel[0][1];
        sum += (double)fLuminance.toLuminance(*p3) * kernel[0][2];

        sum += (double)fLuminance.toLuminance(*p4) * kernel[1][0];
        sum += (double)fLuminance.toLuminance(*p5) * kernel[1][1];
        sum += (double)fLuminance.toLuminance(*p6) * kernel[1][2];

        sum += (double)fLuminance.toLuminance(*p7) * kernel[2][0];
        sum += (double)fLuminance.toLuminance(*p8) * kernel[2][1];
        sum += (double)fLuminance.toLuminance(*p9) * kernel[2][2];
        
        /*
        sum += (double)fLuminance.toLuminance(*p1) * kernel[0][0];
        sum += (double)fLuminance.toLuminance(*p2) * kernel[1][0];
        sum += (double)fLuminance.toLuminance(*p3) * kernel[2][0];

        sum += (double)fLuminance.toLuminance(*p4) * kernel[0][1];
        sum += (double)fLuminance.toLuminance(*p5) * kernel[1][1];
        sum += (double)fLuminance.toLuminance(*p6) * kernel[2][1];

        sum += (double)fLuminance.toLuminance(*p7) * kernel[0][2];
        sum += (double)fLuminance.toLuminance(*p8) * kernel[1][2];
        sum += (double)fLuminance.toLuminance(*p9) * kernel[2][2];
        */

        /*
        for (int i = -1; i <= 1; i++) // iterating rows
        {
            for (int j = -1; j <= 1; j++) // iterating column
            {
                auto muller = kernel[i + 1][j + 1];
                int x1 = x + j;
                int y1 = y + i;

                auto c = fWrapped.getPixel(x1, y1);
                auto lum = fLuminance.toLuminance(c);

                // multiply by the value found in the kernel at [i+1][j+1]
                sum += ((double)lum * muller);

            }
        }
        */

        // After it's all done, take the sum and divide by the overall contribution
        // to get the color value
        sum = abs(sum / kernelContrib);
        uint8_t alum = static_cast<uint8_t>(sum);

        return PixelRGBA(alum,alum,alum);
    }
};

std::shared_ptr<RenderContext> ctx = nullptr;
std::shared_ptr<ScreenSnapshot> screenSamp = nullptr;
std::shared_ptr<GraySampler> gSamp = nullptr;
std::shared_ptr<ConvolutionPixelMap> convSamp = nullptr;


void onFrame()
{
    screenSamp->next();

    //ctx->rectangle(0, 0, canvasWidth, canvasHeight, *screenSamp);
    ctx->rectangle(0, 0, canvasWidth, canvasHeight, *convSamp);
}

void setup()
{
	setCanvasSize(1290, 1024);
	//setFrameRate(30);

	// display screen sampler
	screenSamp = std::make_shared<ScreenSnapshot>(0, 0, canvasWidth, canvasHeight);

    gSamp = std::make_shared<GraySampler>(screenSamp);

    // setup convolution sampler
    convSamp = std::make_shared<ConvolutionPixelMap>(*screenSamp);

	// A rendering context for convenience
	ctx = std::make_shared<RenderContext>(gAppSurface);
}