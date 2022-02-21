/* COP 3502C Programming Assignment3
This program is written by: Jarod Gomberg */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct coordinate //struct for coordinate points
{
  int x, y;
} coordinate;

coordinate myLocation; //global variable for my location

//function to read data from file and return the given points to be sorted
void ReadData(FILE* fp, coordinate point[], int n)
{
  int i = 0;

  while(i < n) //keep looping until all points are read
  {
    fscanf(fp, "%d %d", &point[i].x, &point[i].y);
    i++;
  }
}

//function for comparing coordinate pointers.
//Returns specified values depending on which point values are larger
//if p1 < p2 the function returns < 0
// while p1 and p2 are equal, it returns 0
//and greater than 0 if p1 > p2
int compareTo(coordinate *ptrP1, coordinate *ptrP2)
{

  //find the distance from location for p1 & p2
  float d1 = sqrt((float)pow(myLocation.x-ptrP1->x,2) + (float)pow(myLocation.y-ptrP1->y, 2));
  float d2 = sqrt((float)pow(myLocation.x-ptrP2->x,2) + (float)pow(myLocation.y-ptrP2->y, 2));


  if(d1 < d2) //return -1 if p1 distance is less than p2
    return -1;

  else if(d1 > d2) //return 1 if p1 distance is greater than p2
    return 1;

  else //for equal distances, compare x values first, compare y next
  {
    if(ptrP1->x < ptrP2->x) //return -1 if p2 has greater x
      return -1;

    else if(ptrP1->x > ptrP2->x) //return 1 if p1 has greater x
      return 1;

    else //for equal x's, compare y's
    {
      if(ptrP1->y < ptrP2->y)
        return -1;

      else if(ptrP1->y > ptrP2->y)
        return 1;

      else //if x's and y's are all equal, the points are the same, so return 0
        return 0;
    }
  }
}

//insertion sort function to get input of array from low to high
void insertionSort(coordinate point[], int low, int high)
{
  int i, j;

  for(i = low+1; i <= high; i++)
  {
    j = i-1;
    coordinate key = point[i];

    //using compare function to determine value, insert elements ahead of current if greater than key
    while(j >= low && compareTo(&key, &point[j]) < 0)
    {
      point[j+1] = point[j];
      j--;
    }
    point[j+1] = key;
  }
}

//function that merges previously sorted groups of points to obtain fully sorted array of points
void merge(coordinate point[], int low, int mid, int high)
{
  coordinate temp[high-low+1]; //temporary array of size of left and right sides of array

  int i = low;
  int j = mid+1;
  int k = 0; //variables to initalize start of left, right, & temp array index's

  while(i <= mid && j <= high) //loop through each side of array
  {
    if(compareTo(&point[i], &point[j]) <= 0) //if left side smaller or equal to right
    {
      temp[k] = point[i]; //element is placed in temp
      i++;
    }
    else //otherwise place in temp from right
    {
      temp[k] = point[j];
      j++;
    }
    k++;
  }

  //insert remainder of elements from either side of array to temp
  while(i <= mid)
  {
    temp[k] = point[i];
    k++;
    i++;
  }

  while(j <= high)
  {
    temp[k] = point[j];
    k++;
    j++;
  }

  for(i = low, k = 0; i <= high; i++, k++) //copy all elements from sorted temp to point array
  {
    point[i] = temp[k];
  }
}

//function that recursively sorts points through mergeSort
void mergeSort(coordinate point[], int low, int high, int thresh)
{
  int n = (high - low) + 1; //obtain size n

  if(n <= thresh) //less than threshold, insertionSort points
  {
    insertionSort(point, low, high);
  }
  else
  {
    int mid = (low+high)/2;
    mergeSort(point, low, mid, thresh);
    mergeSort(point, mid+1, high, thresh);
    merge(point, low, mid, high); //if larger than threshold, call mergeSort for left and right sides
                                 //then merge left and right after sorting
  }
}

//sort function for all coordinates
void sort(coordinate point[], int n, int thresh)
{
  if(n <= thresh)
  {
    insertionSort(point, 0, n-1); //if size less than threshold, use insertion
  }
  else
  {
    mergeSort(point, 0, n-1, thresh); //if greater, then use mergeSort
  }
}

//function that searches through points for key and if found, returns the given index or returns -1 if not
int binarySearch(coordinate point[], int n, coordinate key)
{
  int low = 0, high = n-1; //low becomes first and high becomes last element of point

  while(low <= high) //loop through point
  {
    int mid = (low + high)/2; //find mid index

    float distance = compareTo(&key, &point[mid]); //use compare to obtain key distance from mid
    if(distance == 0)
      return mid; //return mid if found

    else if(distance < 0) //begin search in left side
      high = mid - 1;

    else
      low = mid + 1; //otherwise search right
  }
  //for search and key not found
  return -1;
}

int main()
{
  //open input and output file, set to read and write
  FILE* inFile = fopen("assignment3input.txt", "r");
  FILE* outFile = fopen("out.txt", "w");

  //declare array and key variables
  coordinate point[1000000];
  coordinate key;

  int n, s, t, i; //variables for scanning input text

  fscanf(inFile, "%d %d %d %d %d", &myLocation.x, &myLocation.y, &n, &s, &t); //scan file for coordinates

  ReadData(inFile, point, n); //read in coordinates for array
  sort(point, n, t); //sort all points

  for(i = 0; i < n; i++)
  {
    fprintf(outFile, "%d %d\n", point[i].x, point[i].y); //write sorted points to file
  }

  for(i = 0; i < s; i++) //search for s and write to file
  {
    fscanf(inFile, "%d %d", &key.x, &key.y);
    int search = binarySearch(point, n, key);

    if(search == -1)
      fprintf(outFile, "%d %d not found\n", key.x, key.y); //key not found

    else //if found write rank to outfile at index + 1
      fprintf(outFile, "%d %d found at rank %d\n", key.x, key.y, search+1);
  }
  //close in and out files
  fclose(inFile);
  fclose(outFile);

  return 0; //end
}
