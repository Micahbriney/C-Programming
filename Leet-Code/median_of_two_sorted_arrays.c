double getMedian(int *nums, int numsSize);

double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size) {
    int *mergedNums, *head;
    int i, j;

    if(NULL == (mergedNums = malloc(sizeof(int) * (nums1Size + nums2Size)))){
        perror("findMedianSortedArrays() malloc mergedNums()");
        exit(-1);
    }
    // Case 1: nums1 or 2 is empty
    if(nums1Size == 0){
        if(nums2Size == 0) // Both sorted arrays are empty
            return 0.0;
        else
            return getMedian(nums2, nums2Size);
    }
    else if(nums2Size == 0)
        return getMedian(nums1, nums1Size);


    i = j = 0;
    head = mergedNums;
    while(i < nums1Size && j < nums2Size){
        // Case 2: nums1 and nums2 arrays are populated.
        if(nums1[i] < nums2[j]){
            *mergedNums = nums1[i++]; // Store the num
            mergedNums++;             // Increment mergedNums pointer
        }
        else{
            *mergedNums = nums2[j++]; // Store the num
            mergedNums++;             // Increment mergedNums pointer
        }
    }

    // Complete mergedNums array by tacking on the leftovers of num1 or num2
    if(i == nums1Size)
        memcpy(mergedNums, nums2 + j, sizeof(int) * (nums2Size - j));
    else
        memcpy(mergedNums, nums1 + i, sizeof(int) * (nums1Size - i));

    return getMedian(head, nums1Size + nums2Size);
}

double getMedian(int *nums, int numsSize){
    if(numsSize % 2 == 0) // Even. Find average of the two middle nums.
        return (double)(nums[numsSize / 2 - 1] + nums[numsSize / 2]) / 2.0;
    else // Odd.
        return (double)nums[numsSize / 2];
}