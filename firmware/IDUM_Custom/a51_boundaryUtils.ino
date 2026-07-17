//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Functions that help to determine the mode boundaries.  I totally didn't get any help writing this.
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//finds the midpoint between possible mode boundaries

unsigned int CalculateCustomBoundary(unsigned int leftModeIndex, unsigned int rightModeIndex) {
  leftModeIndex *= 2;
  rightModeIndex *= 2;
  unsigned int middle = (leftModeIndex + rightModeIndex) / 2;
  unsigned int value = (middle < BOUNDARIES_2X_SIZE) ? BOUNDARIES_2X[middle] : 0;
  return value;
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Goes through the active modes one by one and calculates the boundaries for the mode ADC value that will cause the mode to change.

void SetBoundariesBetweenActiveModes() {
  unsigned int intervalCount = 0;
  // 0. Erase interval settings:
  initializeIntervals();

  // 1. First pass: for each active mode add it to the next available interval:
  for (int i = 0; i < MODE_COUNT; ++i) {
    if (activeModes[i]) {
      INTERVALS[intervalCount++].mode = i;
    } else {
      // TODO: Potentially add mode to ignore list.
    }
  }

  // 2. Loop through each interval and find boundaries from custom table:
  for (int i = 0; i < intervalCount; ++i) {
    if (i == 0) {
      // The first interval's right side is always the lowest boundary.
      INTERVALS[i].left = 0;
    } else {
      // The left side of this interval needs to be halfway to its neighboring left interval's right side.
      INTERVALS[i].left = CalculateCustomBoundary(INTERVALS[i - 1].mode, INTERVALS[i].mode);
    }

    if (i == intervalCount - 1) {
      // The last interval's right side is always the highest boundary.
      INTERVALS[i].right = BOUNDARIES_2X[BOUNDARIES_2X_SIZE - 1];
    } else {
      // The right side of this interval needs to be halfway to its neighboring right interval's left side.
      INTERVALS[i].right = CalculateCustomBoundary(INTERVALS[i].mode, INTERVALS[i + 1].mode);
    }
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//Puts the left and rightmost variables to their furthest values and then stores the very important beef

void initializeIntervals() {
  for (int i = 0; i < INTERVAL_COUNT; ++i) {
    INTERVALS[i].left = 0;
    INTERVALS[i].right = MAX_INTERVAL_BOUNDARY;
    INTERVALS[i].mode = 0xBEEF;
  }
}

//////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
