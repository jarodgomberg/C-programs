//jarod gomberg
//COP3502C-21
//assignment1
//June 6, 2021

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student //provided student struct
{
  int id;
  char *lname;
  float *scores;
  float std_avg;
} student;

typedef struct course //provided course struct
{
  char *course_name;
  int num_sections;
  student **sections;
  int *num_students;
  int *num_scores;
} course;

student **read_sections(FILE *fp, int num_students[], int num_scores[], int num_sections) //takes data from each section in a course, while filling students and scores per each section. Rerturns fully loaded "sections" array
{
  student **all_sections = (student**)malloc(sizeof(student*)*num_sections); //allocate memory for all sections in course
  char last_name[20];

  for(int i = 0; i < num_sections; i++) //iterate through sections
  {
    fscanf(fp, "%d %d", &num_students[i], &num_scores[i]); //read file for student and score info
    all_sections[i] = (student*)malloc(sizeof(student)*num_students[i]); //allocate for students in each section
    for(int j = 0; j < num_students[i]; j++) //iterate through students
    {
      fscanf(fp, "%d %s", &all_sections[i][j].id, last_name); //find name
      all_sections[i][j].lname = malloc(strlen(last_name)+1);
      strcpy(all_sections[i][j].lname, last_name);

      all_sections[i][j].scores = (float *)malloc(sizeof(float) * num_scores[i]); //allocate memory for scores of each student

      float total_scores = 0;
      for(int k = 0; k < num_scores[i]; k++) //iterate through scores
      {
        fscanf(fp, "%f", &all_sections[i][j].scores[k]);
        total_scores += all_sections[i][j].scores[k];
      }
      all_sections[i][j].std_avg = total_scores/num_scores[i];  //determine average scores for students in sections
    }
  }
  return all_sections; //return all sections data
}

course *read_courses(FILE *fp, int *num_courses) //reads file for test case information, then allocates memory, fills, and returns loaded courses
{
  fscanf(fp, "%d", num_courses); //get number of courses

  course *all_courses = (course*)malloc(sizeof(course)* *num_courses); //allocate memory for each course
  char course_title[20]; //for course name
  int i;
  for(i = 0; i < *num_courses; i++) //iterate through courses
  {
    fscanf(fp, "%s", course_title); //find course name
    int length = strlen(course_title);

    all_courses[i].course_name = malloc(length+1); //allocate enough memory for name and for copying
    strcpy(all_courses[i].course_name, course_title);

    fscanf(fp, "%d", &all_courses[i].num_sections); //get number of sections
    all_courses[i].num_students = (int*)malloc(sizeof(int) * all_courses[i].num_sections); // allocate memory for number of students in each section of specific course and do the same for number of scores in that course
    all_courses[i].num_scores = (int*)malloc(sizeof(int) * all_courses[i].num_sections);

    all_courses[i].sections = read_sections(fp, all_courses[i].num_students, all_courses[i].num_scores, all_courses[i].num_sections); //call read sections to fill all section and student data into courses
  }

  return all_courses;

}

void process_sections(student **sections, int num_students[], int num_scores[], int num_sections) //prints section of course data
{
  float *sec_std_avg = (float *)malloc(num_sections * sizeof(float)); //allocate memory for the average of each section
  int max_stu_avg = 0; //student highest average
  int max_sec_avg = 0; //section highest average
  int pass_count = 0; //count grades that passed
  float max_avg = -1; //store current high average
  int i, j, k;


  for(i = 0; i < num_sections; i++) //iterate through sections
  {
    float sec_total = 0; //to get all averages of the section, later used for standard avg
    for(j = 0; j < num_students[i]; j++)
    {
      if(sections[i][j].std_avg >= 70) //if passing
      {
        pass_count++;
      }
      sec_total += sections[i][j].std_avg; //add all student averages
      if(sections[i][j].std_avg > max_avg)
      {
        max_avg = sections[i][j].std_avg;
        max_stu_avg = j;
        max_sec_avg = i;
      } //get highest average
    }
    sec_std_avg[i] = sec_total/num_students[i];
  }
  printf("%d ", pass_count);
  for(k = 0; k < num_sections; k++)
  {
    printf("%.2f ", sec_std_avg[k]);
  }
  printf("%d %s %.2f\n", sections[max_sec_avg][max_stu_avg].id, sections[max_sec_avg][max_stu_avg].lname, sections[max_sec_avg][max_stu_avg].std_avg);
  free(sec_std_avg); //release memory for standard average of section
}

void process_courses(course *courses, int num_courses) //prints course data
{
  for(int i = 0; i < num_courses; i++)
  {
    printf("%s ", courses[i].course_name);
    process_sections(courses[i].sections, courses[i].num_students, courses[i].num_scores, courses[i].num_sections); //calls function for section info and printing
  }
}

void free_all_sections(student **sections, int num_sections, int *num_students, int *num_scores) //frees all memory used for section info
{
  for(int i = 0; i < num_sections; i++) //iterate through all sections
  {
    for(int j = 0; j < num_students[i]; j++) //iterate through students and free name and scores
    {
      free(sections[i][j].lname);
      free(sections[i][j].scores);
    }
    free(sections[i]); //free iterated section
  }
  free(sections); //free all sections
}

void release_courses(course *courses, int num_courses) //frees all memory used for course info
{
  for(int i = 0; i < num_courses; i++) //iterate through courses
  {
    free(courses[i].course_name);
    free_all_sections(courses[i].sections, courses[i].num_sections, courses[i].num_students, courses[i].num_scores); //calls function to free section info of each course
    free(courses[i].num_students); //free number students
    free(courses[i].num_scores); //free number of scores
  }
  free(courses); //free all courses
}


int main()
{
  FILE *fp = fopen("assignment1input.txt", "r"); //open and read data from input file
  if(fp == 0) //if file does not exist
  {
    perror("fopen");
    exit(-1);
  }

  //declare variables for array of courses and number of test cases
  course *courses;
  int test_cases;
  fscanf(fp, "%d", &test_cases); //get number of test cases
  for(int i = 1; i <= test_cases; i++) //for each test case
  {
    //declare variable to follow number of courses
    int num_courses;
    printf("\ntest case %d\n", i);
    courses = read_courses(fp, &num_courses); //call all functions
    process_courses(courses, num_courses);
    release_courses(courses, num_courses);

    printf("\n"); //new line for clarity in output
  }
  fclose(fp); //close file
  return 0;
}
