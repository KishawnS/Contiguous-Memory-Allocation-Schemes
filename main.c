#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Global Variables
int numberOfProcesses = 0;

// Requesting a contiguous block of memory
int allocate() {
  int size = (rand() % 25) + 1; // generates random number from 1-25 (4KB-100KB)
  numberOfProcesses++;
  return size;
}

// Releasing a contiguous block of memory
void release(int* array, int removeNum) {
  int counter = 0;        // counter for number of processes checked
  int processErasing = 0; // flag for having found process to be released
  for (int i = 0; i < 256; i++) {
    // If intended process fully released, exit the for loop
    if (counter == removeNum && processErasing == 1 &&
        (array[i] == 0 || array[i] == 1)) {
      break;
    }
    // If non-first indices of intended process, release
    else if (counter == removeNum && processErasing == 1) {
      array[i] = 0;
    }
    // If first index of intended process reached, release and set flag
    else if (counter == removeNum && array[i] == 1) {
      array[i] = 0;
      processErasing = 1;
    }
    // If intended process not reached, search for next process
    else if (array[i] == 1) {
      counter++;
    }
  }
}

// Reporting the regions of free and allocated memory
void status(int* array) {
  int counter = 0; // counter for indices of array
  int hole = 0;    // flag for checking a hole
  int process = 0; // flag for checking a process
  int start, end;  // start and end indices of a process or hole
  int holeCount = 0, processCount = 0;  // counters for number of processes and holes
  while (counter < 256) {
    // If a hole is at index 0, set the start index and flag for hole
    if (array[counter] == 0 && hole == 0 && process == 0) {
      start = counter;
      hole = 1;
    }
    // If a hole just ended and process started, switch flag to process and
    // print previous hole status
    else if (array[counter] == 1 && hole == 1) {
      hole = 0;
      process = 1;
      holeCount++;
      printf("Byte %d - Byte %d : Hole #%d (%d KB)\n", start * 4096,
             (end + 1) * 4096, holeCount, (end - start + 1) * 4);
      start = counter;
    }
    // If a process is at index 0, set the start index and flag for process
    else if (array[counter] == 1 && process == 0) {
      start = counter;
      process = 1;
    }
    // If a process just ended, switch or keep flag as needed and print previous
    // process status
    else if (process == 1 && (array[counter] == 0 || array[counter] == 1)) {
      if (array[counter] == 0) {
        process = 0;
        hole = 1;
      }
      processCount++;
      printf("Byte %d - Byte %d : Process #%d (%d KB)\n", start * 4096,
             (end + 1) * 4096, processCount, (end - start + 1) * 4);
      start = counter;
    }
    end = counter;
    counter++;
  }
  // Displaying last region of memory
  if (process == 1) {
    processCount++;
    printf("Byte %d - Byte %d : Process #%d (%d KB)\n", start * 4096,
           (end + 1) * 4096, processCount, (end - start + 1) * 4);
  } else if (hole == 1) {
    holeCount++;
    printf("Byte %d - Byte %d : Hole #%d (%d KB)\n", start * 4096, (end + 1) * 4096,
           holeCount, (end - start + 1) * 4);
  }
  printf("Total Number of Processes: %d\nTotal Number of Holes: %d\n", 
          processCount, holeCount);
}

void first_fit(int* array, int size) {
  int counter = 0;            // counter for indices of array
  int space;                  // counter for space in hole being checked
  while (counter < 256) {
    space = 0;                // reset space counter to 0 for every next hole
    // While checking each hole, increment indices and space counter
    while (array[counter] == 0) {
      // If the hole has enough space for the process to be added, it is added
      if (space == size) {
        for (int i = counter - size; i < counter; i++) {
          array[i] = size + i - counter + 1;
        }
        break;
      }
      space++;
      counter++;
    }
    if (space == size) {
      break;
    } else {
      counter++;
    }
  }
  // If there are no holes with enough space, display an error message
  if (space != size) {
    printf("There was no available space in the system for %d KB of memory.\n",
           size * 4);
    numberOfProcesses--;
  }
}

void best_fit(int* array, int ProcessSize) {
  // iterate through list find the one that has the smallest difference in hole
  // size and process size and insert the process there
  int start, end;
  int counter = 0, hole = 0, currentIndex = 0, tempSize = 0, tempPosition = 0,
      process = 0;
  
  // iterate through the entire list
  while (counter < 256) {
    // If a hole is at index 0, set the start index and flag for hole
    if (array[counter] == 0 && hole == 0 && process == 0) {
      start = counter;
      tempPosition = start;
      hole = 1;
    }
    // If a hole just ended and process started, switch flag to process
    // check if the available space is sufficient for the process
    else if (array[counter] == 1 && hole == 1) {
      hole = 0;
      process = 1;
      // check if the available space is sufficient for the process
      if (ProcessSize <= (end-start+1)) {
        // by default we will set the temp size and position to the sufficient
        // hole
        if (tempSize == 0) {
          tempSize = (end-start+1);
          tempPosition = start;
        }
        // we will check if the new hole available memory is smaller than the
        // size of the previous hole, this is so we have the least amount of
        // empty space
        else if (tempSize > (end-start+1)) {
          tempSize = (end-start+1);
          tempPosition = start;
        }
      }
      start = counter;
    }
    // If a process is at index 0, set the start index and flag for process
    else if (array[counter] == 1 && process == 0) {
      start = counter;
      process = 1;
    }
    // If a process just ended, switch or keep flag as needed and print previous
    // process status
    else if (process == 1 && (array[counter] == 0 || array[counter] == 1)) {
      if (array[counter] == 0) {
        process = 0;
        hole = 1;
      }
      start = counter;
    }
    end = counter;
    counter++;
  }
  if (hole == 1) {
    if (ProcessSize<(end-start+1)){
      if (tempSize == 0){
        tempSize = (end-start+1);
        tempPosition = start;
      }
      else if (tempSize > (end - start+1)) {
        tempSize = (end - start+1);
        tempPosition = start;
      }
    }
  }
  // fill up the memory to the locations we determined previously
  if (tempSize > 0) { // check if there is a hole available
    int counter = 1;
    // add the process into the system
    for (int i = tempPosition; i < tempPosition + ProcessSize; i++) {
      array[i] = counter;
      counter++;
    }
  } else {
    printf("There is not enough space in the system for %d KB of memory.\n",
           ProcessSize * 4);
    numberOfProcesses--;
  }
}

void worst_fit(int* array, int ProcessSize) {
  // iterate through list find the one that has the smallest difference in hole
  // size and process size and insert the process there
  int start, end;
  int counter = 0, hole = 0, currentIndex = 0, tempSize = 0, tempPosition = 0,
      process = 0;
  
  // iterate through the entire list
  while (counter < 256) {
    // If a hole is at index 0, set the start index and flag for hole
    if (array[counter] == 0 && hole == 0 && process == 0) {
      start = counter;
      tempPosition = start;
      hole = 1;
    }
    // If a hole just ended and process started, switch flag to process
    // check if the available space is sufficient for the process
    else if (array[counter] == 1 && hole == 1) {
      hole = 0;
      process = 1;
      // check if the available space is sufficient for the process
      if (ProcessSize <= (end-start+1)) {
        // by default we will set the temp size and position to the sufficient
        // hole
        if (tempSize == 0) {
          tempSize = (end-start+1);
          tempPosition = start;
        }
        // we will check if the new hole available memory is smaller than the
        // size of the previous hole, this is so we have the least amount of
        // empty space
        else if (tempSize < (end-start+1)) {
          tempSize = (end-start+1);
          tempPosition = start;
        }
      }
      start = counter;
    }
    // If a process is at index 0, set the start index and flag for process
    else if (array[counter] == 1 && process == 0) {
      start = counter;
      process = 1;
    }
    // If a process just ended, switch or keep flag as needed and print previous
    // process status
    else if (process == 1 && (array[counter] == 0 || array[counter] == 1)) {
      if (array[counter] == 0) {
        process = 0;
        hole = 1;
      }
      start = counter;
    }
    end = counter;
    counter++;
  }
  if (hole == 1) {
    if (ProcessSize<(end-start+1)){
      if (tempSize == 0){
        tempSize = (end-start+1);
        tempPosition = start;
      }
      else if (tempSize < (end - start+1)) {
        tempSize = (end - start+1);
        tempPosition = start;
      }
    }
  }
  // fill up the memory to the locations we determined previously
  if (tempSize > 0) { // check if there is a hole available
    int counter = 1;
    // add the process into the system
    for (int i = tempPosition; i < tempPosition + ProcessSize; i++) {
      array[i] = counter;
      counter++;
    }
  } else {
    printf("There is not enough space in the system for %d KB of memory.\n",
           ProcessSize * 4);
    numberOfProcesses--;
  }
}

void compaction(int* array) {
  int counter = 0, holeCount = 0;   // index variables
  // iterates through the array until hole at the end
  while (counter < 256-holeCount) { 
    // left shifts the array when a hole is reached
    if (array[counter] == 0) {
      holeCount++;
      for (int i = counter; i < 256-holeCount; i++) {  
        array[i] = array[i+1];
      }
      array[256-holeCount] = 0;
    }
    else {counter++;}
  }
}

int main(int argc, char* argv[]) {
  // Command line arguments
  int compact = atoi(argv[1]);  // determines if compaction is used
  int fit = atoi(argv[2]);      // determines which fit is used
  
  // Initializing array simulating 1MB of data; 256 4KB spaces
  int *array = (int*) malloc(256*sizeof(int));
  srand(time(NULL));

  // Filling the entire array with random size processes
  int currentIndex = 0;
  while (currentIndex < 256) {
    int size = allocate();
    int counter = 1;
    if (size > 255 - currentIndex + 1) {
      size = 255 - currentIndex + 1;
    } 
    for (int i = currentIndex; i < currentIndex + size; i++) {
      array[i] = counter;
      counter++;
    }
    currentIndex += size;
  }
  printf("\n----- Filled Array -----\n");
  status(array);

  // Removing 10% of processes randomly from the array
  int numToRemove = round(numberOfProcesses / 10);
  while (numToRemove) {
    int removeNum = rand() % numberOfProcesses;
    release(array,removeNum);
    numToRemove--;
  }
  printf("\n----- Array After 10%% of Processes Removed -----\n");
  status(array);

  // Enters if user indicates for compaction to be used
  if (compact) {
    printf("\n----- Array After Compaction -----\n");
    compaction(array);
    status(array);
  }
  // Generating the size of the process to be added
  int newProcessSize = allocate(); 
  if (fit == 1){  // First Fit allocation
    printf("\n----- First Fit Allocation after adding %d KB ----- \n",
            newProcessSize * 4);
    first_fit(array,newProcessSize);
  }
  else if (fit == 2) {  // Best fit allocation
    printf("\n----- Best Fit Allocation after adding %d KB ----- \n",
           newProcessSize * 4);
    best_fit(array,newProcessSize);
  }
  else if (fit == 3) {  // Worst fit allocation 
    printf("\n----- Worst Fit Allocation after adding %d KB ----- \n",
           newProcessSize * 4);
    worst_fit(array,newProcessSize);
  }
  status(array);

  free(array);
  return 0;
}