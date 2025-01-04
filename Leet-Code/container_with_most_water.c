#define MIN(x,y) (((x) < (y)) ? (x) : (y))

int maxArea(int* height, int heightSize) {
    int i, maxArea = 0;
    int k = heightSize - 1;

    while(i != k){
        if(maxArea < ((k - i) * MIN(height[i], height[k])))
            maxArea = ((k - i) * MIN(height[i], height[k]));
        height[i] > height[k] ? k-- : i ++;
    }
    return maxArea;
}