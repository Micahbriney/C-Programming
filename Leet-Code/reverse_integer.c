int reverse(int x){
    long long rInt = 0;

    if(x == 0)
        return x;

    while(x != 0){
        rInt = (rInt * 10) + (x % 10);
        x = x / 10;
    }

    if(rInt > INT_MAX || rInt < INT_MIN)
        return 0;

    return (int)rInt;
}