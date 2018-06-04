#include <string>
#include <vector>

#include "VolumeViewer.h"
#include "colorMap.h"

#include <vtkSmartPointer.h>
#include <vtkTIFFReader.h>
#include <vtkImageData.h> 

#include <vtkGPUVolumeRayCastMapper.h>
//#include <vtkColorTransferFunction.h>
//#include <vtkPiecewiseFunction.h>
//#include <vtkVolumeProperty.h>

#include <vtkActor.h>

#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>

#include <vtkDiskSource.h>

//#include <.h>
#include <math.h>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cassert>
#include <random>
#include <unordered_map>
#include <fstream>

#define pi 3.1415

vtkSmartPointer<vtkImageData> readTiffImage(std::string filename,bool);
vtkSmartPointer<vtkActor> getClyinder(float xoff, float yoff, float zoff, int angle, float rad);
vtkSmartPointer<vtkActor> getDisk(float xoff, float rad);
std::vector< vtkSmartPointer<vtkActor>> getPlatelet(std::vector <vtkSmartPointer<vtkActor>> cylinactors, int xsize, float xoff, float yoff, float zoff, float rad, int theta);

class Center3D{
  public:
  Center3D (float x, float y, float z) : x(x), y(y), z(z){};
  
  float x, y, z;
  };

struct hashFunc{
    size_t operator()(const Center3D &k) const{
    size_t h1 = std::hash<float>()(k.x);
    size_t h2 = std::hash<float>()(k.y);
    size_t h3 = std::hash<float>()(k.z);
    return (h1 ^ (h2 << 1)) ^ h3;
    }
};

struct equalsFunc{
  bool operator()( const Center3D& lhs, const Center3D& rhs ) const{
    return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
  }
};
typedef std::unordered_map< Center3D , int, hashFunc, equalsFunc> hexMap;

int main(int argc, char* argv[])
{
  VolumeViewer* V = new VolumeViewer();
  V->setBackgroundColor( 214/255., 234/255., 248/255. );
  
  int option = 2;

  int xsize = 3;
  int ysize = 4;
  int zsize = 4;
  float rad;
  std::cout << "Enter the radius:";
  std::cin >> rad;
  float xoffset = 2*sqrt(0.03);//because radius=0.2
  //float xoffset = 2* sqrt((rad*rad) + (rad/2)*(rad/2));
  float yoffset = 0.3;
  //float yoffset = rad + (rad/2);
  float zoffset = 2*rad; 
  float xoff = 0; float yoff = 0; float zoff= 0;
  //Read the image
  /*
  vtkSmartPointer<vtkImageData> image = readTiffImage(argv[1],false);
  image->SetSpacing(0.6215,0.6215,1);
  int* dims = image->GetDimensions();
  */
  
  //creating initial substrate disk
  vtkSmartPointer<vtkActor> diskActor = getDisk(xsize,rad);
  
  std::vector< vtkSmartPointer<vtkActor> > cylinderactors;
 // for (int i = 0; i < numCylinders; i++)
  cylinderactors.push_back(diskActor);
  switch(option){
  // when no amino acid present
  case 1:
  //int ycounter = 0;
  for (int z = 0; z < zsize; z++)
  {
  int ycounter = 0;
  for (int y = 0; y < ysize; y++)
  {
    for (int x = 0; x < xsize; x++)
    {
      cylinderactors.push_back(getClyinder(xoff,yoff,zoff,0, rad));
      xoff += xoffset;
    }
    ycounter += 1;
    if (ycounter % 2 == 0)
      xoff = 0;
    else
      xoff = xoffset/2;      

    yoff += yoffset;
  }
  xoff = 0; 
  //yoff = rad;
  yoff = 0;
  zoff += zoffset;
  }

  //vtkSmartPointer<vtkActor> clyinderActor = getClyinder();

  //std::vector< vtkSmartPointer<vtkActor> > actors;
  //Add objects to be visualized to the viewer
  //actors.push_back(clyinderActor);

  for(size_t i = 0; i < cylinderactors.size(); ++i){
      V->addActor(cylinderactors[i]); 
  }
  
  //setup viewing window....
  V->getWindowInteractor()->Initialize();
  V->getRenderWindow()->Render(); 
  V->getWindowInteractor()->Start();

  //when L-asp amino-acid is present
  case 2: 
  yoff  = 0; float xedge = 0; float yedge = 0;
  std::vector< vtkSmartPointer<vtkActor> > casetwocylinderactors;
  /*for (int x = 0; x < xsize; x++)
  {
    casetwocylinderactors.push_back(getClyinder(xoff,yoff,zoff, anginc));
    //xoff = xoff + (xoffset/2)  + (xoffset/2)*cos(4*pi/180) + (rad/2)*tan(4*pi/180);
    //yoff = yoff + (xoffset/2)*sin(4*pi/180);
    //xoff = (n * xoffset) + (xoffset/2)  + (xoffset/2)*cos(anginc*pi/180) + (rad/2)*tan(anginc*pi/180);
    //yoff = (xoffset/2)*sin(anginc*pi/180);
    xoff =  (2*rad*cos(28*pi/180)*cos(2*pi/180));
    yoff =  (2*rad*cos(28*pi/180)*sin(2*pi/180));
    x2 = xoff + cos(4*pi/180)*xoff - sin(4*pi/180)*yoff;
    y2 = yoff + sin(4*pi/180)*xoff + cos(4*pi/180)*yoff;
    anginc += 4;
    //n++;
 }
  casetwocylinderactors.push_back(getClyinder(x2,y2,zoff,anginc));
 */
  std::vector <vtkSmartPointer<vtkActor>> cylinactors;
  
  hexMap freeCenters;
 
  std::random_device rd;
  std::mt19937 eng(rd());
  std::uniform_int_distribution<> distr(1,6);
  std::vector <Center3D> list_of_centers;
  std::vector <int> list_angles;
  std::vector <int> list_of_numbers;
  
  /*std::uniform_int_distribution <> disto(1,50);
  std::ofstream myfile("randomnos.txt");
  myfile.open("randomnos.txt", std::ios::out | std::ios::app);
  int r = 0;
  if (myfile.is_open())
  {
   for (int i = 0; i < 1000; i++)
   {
    r = disto(eng);
    myfile << r << "\n";
   }
   myfile.close();
  }
  else
  { std::cout << "unable to open file";}*/
  
  //int angles[] = {-120, -60, 0, 60, 120, 180};
  
  int angles[] = {0, 60, 120, 180, 240, 300};
  int interval;
  std::cout << "enter the number of mother hexagons:";
  std::cin >> interval;
  for (int theta = 0; theta < 360; theta += 360/interval)
  {
    //radially outward growth of platelets
    //cylinactors = getPlatelet(casetwocylinderactors, xsize, cos(theta*pi/180), sin(theta*pi/180), rad*sin(6*pi/180), rad, theta);
    
    //random growth of platelets
    cylinactors.push_back(getClyinder( cos(theta*pi/180), sin(theta*pi/180), rad*sin(6*pi/180)+ (rad*0.3*cos(6*pi/180)), theta + 90 + 4, rad));
    
    xoff = cos(theta*pi/180);
    yoff = sin(theta*pi/180);
    zoff = (rad * sin(6*pi/180)) + (rad*0.3*cos(6*pi/180));
    list_of_centers.push_back(Center3D(xoff, yoff, zoff));
    list_angles.push_back(theta);
    list_of_numbers.push_back(0);
    /*anginc = 4;
    int zcounter = 3;
   
    for (int x = 0; x < xsize; x++) 
    {
    int rnd = distr(eng);
    xedge = xoff + rad*cos((30 + theta + 90 +  angles[rnd-1] + anginc)*pi/180);
    yedge = yoff + rad*sin((30 + theta + 90 +  angles[rnd-1] + anginc)*pi/180);
    xoff = xedge + rad*sin((64 + theta + 90 + angles[rnd-1] + anginc)*pi/180);
    yoff = yedge - rad*cos((64 + theta + 90 + angles[rnd-1] + anginc)*pi/180);
    zoff = zcounter*rad*sin(6*pi/180);
    cylinactors.push_back(getClyinder(xoff,yoff,zoff, theta + 90 + anginc + 4, rad));
    anginc += 4;
    zcounter += 2;
    }*/
   }

  int iter;
  std::cout << "Enter number of iterations:";
  std::cin >> iter;
  for (int i = 0; i < iter; i ++)
  {
    std::uniform_int_distribution<> dist(1,list_of_centers.size());
    int randno = dist(eng);
    int rnd = distr(eng); 
    
    xedge = list_of_centers[randno-1].x + rad*cos((30 + list_angles[randno-1] + 90 +  angles[rnd-1] + 4)*pi/180);
    yedge = list_of_centers[randno-1].y + rad*sin((30 + list_angles[randno-1] + 90 +  angles[rnd-1] + 4)*pi/180);
    xoff = xedge + rad*sin((64 + list_angles[randno-1] + 90 + angles[rnd-1] + 4)*pi/180);
    yoff = yedge - rad*cos((64 + list_angles[randno-1] + 90 + angles[rnd-1] + 4)*pi/180);
     
    if ((angles[rnd-1] == 60) || (angles[rnd-1] == 240))
      zoff = list_of_centers[randno-1].z;
    else if ((angles[rnd-1] == 0) || (angles[rnd-1] == 300))
      zoff = ((((list_of_centers[randno-1].z  - (rad*0.3/sin(84*pi/180))) / (rad * sin(6*pi/180))) + 2 ) * rad * sin(6*pi/180)) + (rad*0.3/sin(84*pi/180));
    else 
      zoff = ((((list_of_centers[randno-1].z  - (rad*0.3/sin(84*pi/180))) / (rad * sin(6*pi/180))) - 2 ) * rad * sin(6*pi/180)) + (rad*0.3/sin(84*pi/180));
    //if (zoff > 0)
      //cylinactors.push_back(getClyinder(xoff,yoff,zoff, list_angles[randno-1] + 90 + 4 + 4, rad));
    
    list_of_numbers[randno-1]++;    
    
    /*int numcount = 0;
    for (Center3D& c : list_of_centers)
    {
    if (sqrt((xoff - c.x)*(xoff - c.x) + (yoff - c.y)*(yoff - c.y) + (zoff - c.z)*(zoff - c.z)) <= (4*rad + 2*rad*tan(4*pi/180)))
    {
     list_of_numbers[numcount]++;
     if (list_of_numbers[numcount] == 6)
     {
       list_of_centers.erase(list_of_centers.begin() + numcount);
       list_angles.erase(list_angles.begin() + numcount);
       list_of_numbers.erase(list_of_numbers.begin() + numcount);
     }
    }
    numcount ++;
    } */   

    //list_angles.push_back(list_angles[randno-1] + 4);

    int flag = 0;
    for (Center3D& c: list_of_centers)
    {
     if ((sqrt((xoff - c.x)*(xoff - c.x) + (yoff - c.y)*(yoff - c.y) ) <= rad))
     {
      flag = 1;
      break; 
     }
    }
    if (flag == 0)
    {
     list_of_centers.push_back(Center3D(xoff, yoff, zoff));
     list_of_numbers.push_back(1);
     list_angles.push_back(list_angles[randno-1] + 4);
     if (zoff > 0)
      cylinactors.push_back(getClyinder(xoff,yoff,zoff, list_angles[randno-1] + 90 + 4 + 4, rad));
    }
  }


  std::cout << "list of centers size:" << list_of_centers.size() << "\n";
  std::cout << "list of angles size:" << list_angles.size() << "\n";

    for (int i = 0; i < cylinactors.size(); i++)
    V->addActor(cylinactors[i]);
  
      
  V->addActor(getDisk(2,rad));

  //setup viewing window....
  V->getWindowInteractor()->Initialize();
  V->getRenderWindow()->Render();
  V->getWindowInteractor()->Start();
 
 }
  return EXIT_SUCCESS;
}


std::vector< vtkSmartPointer<vtkActor>> getPlatelet(std::vector< vtkSmartPointer<vtkActor>> cylinactors, int xsize, float xoff, float yoff, float zoff, float rad, int theta)
{
  float xedge = 0; float yedge = 0; int anginc = 0; int zcounter = 3;
  std::vector <float> xedges;
  std::vector <float> yedges;
  std::vector <float> absedges;
  std::vector <int> ivals;
  for (int x = 0; x < xsize; x++)
  {
    //cylinactors.push_back(getClyinder(xoff,yoff,zoff, anginc, rad, 1));
    for (int i = -120; i <= 180; i+=60) {
    xedge = xoff + rad*cos((30 + theta + 90 +  i + anginc)*pi/180);
    yedge = yoff + rad*sin((30 + theta + 90 +  i + anginc)*pi/180);
    xedges.push_back(xedge);
    yedges.push_back(yedge);
    absedges.push_back(xedge*xedge + yedge*yedge);
    ivals.push_back(i);
    //xoff = xedge + rad*sin((64 + theta + 90)*pi/180);
    //yoff = yedge - rad*cos((64 + theta + 90)*pi/180);
    }
    int index = *std::max_element(absedges.rbegin(), absedges.rend());
    xoff = xedges[index] + rad*sin((64 + theta + 90 + ivals[index] + anginc)*pi/180);
    yoff = yedges[index] - rad*cos((64 + theta + 90 + ivals[index] + anginc)*pi/180);
    zoff = zcounter * rad * sin(6*pi/180);
   
    cylinactors.push_back(getClyinder(xoff,yoff,zoff, theta + 90 + anginc + 4, rad));
    anginc += 4;
    zcounter += 2;
    xedges.clear();
    yedges.clear();
    absedges.clear();
    ivals.clear();
  }
  return cylinactors;
}

//getDisk function
vtkSmartPointer<vtkActor> getDisk(float xoff, float rad){
    /*Copied from vtk wiki*/
    vtkSmartPointer<vtkDiskSource> diskSource = 
      vtkSmartPointer<vtkDiskSource>::New();
    //diskSource->SetCenter((xsize*rad/2),0,0);    
    // Create a mapper and actor.
    vtkSmartPointer<vtkPolyDataMapper> diskMapper = 
      vtkSmartPointer<vtkPolyDataMapper>::New();
    diskMapper->SetInputConnection(diskSource->GetOutputPort());

    vtkSmartPointer<vtkActor> diskActor = 
      vtkSmartPointer<vtkActor>::New();
    diskActor->SetMapper(diskMapper);
    //diskActor->SetPosition(0,0,-rad*sin(6*pi/180)-(rad*0.3*cos(6*pi/180)));
    diskActor->SetPosition(0,0,0);
    //diskActor->SetPosition(xoff,0,-rad/2);
    diskActor->RotateX(90.0);
    diskSource->SetInnerRadius(0);
    diskSource->SetOuterRadius(xoff);
    diskSource->SetCircumferentialResolution(50);
    diskActor->RotateX(90.0);
return diskActor;
}



vtkSmartPointer<vtkActor> getClyinder(float x, float y, float z, int angle, float rad){
    /*Copied from vtk wiki*/
    vtkSmartPointer<vtkCylinderSource> cylinder =
	vtkSmartPointer<vtkCylinderSource>::New();
    cylinder->SetResolution(6);
    cylinder->SetHeight(rad*0.6);
    cylinder->SetRadius(rad);
   // cylinder->SetCenter(0,0.4,0);    

    // The mapper is responsible for pushing the geometry into the graphics library.
    // It may also do color mapping, if scalars or other attributes are defined.
    vtkSmartPointer<vtkPolyDataMapper> cylinderMapper =
	vtkSmartPointer<vtkPolyDataMapper>::New();
    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());

    // The actor is a grouping mechanism: besides the geometry (mapper), it
    // also has a property, transformation matrix, and/or texture map.
    // Here we set its color and rotate it around the X and Y axes.
    vtkSmartPointer<vtkActor> cylinderActor =
	vtkSmartPointer<vtkActor>::New();
    cylinderActor->SetMapper(cylinderMapper);
    int flg = 0;
    for (int theta = 0; theta <= 360; theta += 30)
    {
    if ( (x == cos(theta*pi/180)) && (y == sin(theta*pi/180)) && (z == rad*sin(6*pi/180)+ (rad*0.3*cos(6*pi/180))))
    { flg = 1; std::cout << flg; break; }
    }
    if (flg == 1)
     cylinderActor->GetProperty()->SetColor(0.2000, 0.6300, 0.7900);
    else 
    cylinderActor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
    
    //cylinderActor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
    cylinderActor->GetProperty()->SetOpacity(0.5);
    cylinderActor->RotateX(90.0);
    cylinderActor->RotateY(-30.0+angle);
    cylinderActor->RotateZ(6.0);
    //cylinderActor->RotateY(-30.0);
    cylinderActor->SetPosition(x,y,z);
    return cylinderActor;
}


vtkSmartPointer<vtkImageData> readTiffImage(std::string filename, bool verbose){
  // Read the image
    vtkSmartPointer<vtkTIFFReader> reader =
	vtkSmartPointer<vtkTIFFReader>::New();

  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkSmartPointer<vtkImageData> vtkData =
      vtkSmartPointer<vtkImageData>::New();
  vtkData->ShallowCopy(reader->GetOutput());
  if(verbose){
      std::cout<<"Read Image: "<<filename<<std::endl;
      std::cout<<"Scalar Type: "<<vtkData->GetScalarType()<<std::endl;
      std::cout<<"Scalar Size: "<<vtkData->GetScalarSize()<<std::endl;
      std::cout<<"Scalar Components: "<<vtkData->GetNumberOfScalarComponents()<<std::endl;
  }
  return vtkData;
}

