int myAtoi(char* s) {
    long value = 0;
    int i = 0;
    short sign = 1;

    // if((s == NULL   ||
    //     // s[i] < '0'  ||
    //     // s[i] > '9') &&
    //     // s[i] != ' ' &&
    //     // s[i] != '-' &&
    //     // s[i] != '+')
    //     s* < '0'  ||
    //     s* > '9') &&
    //     s* != ' ' &&
    //     s* != '-' &&
    //     s* != '+')
    //     return value;
    
    if(s[i] == ' ')
        while(s[++i] == ' ')
            continue;
    
    if(s[i] == '-'){
        sign = -1;
        i++;
    }
    else if(s[i] == '+')
        i++;

    while(s[i] >= '0' && s[i] <= '9'){
        value = value * 10 + s[i++] - '0';
        if(value > INT_MAX)
            break;
    }
    
    value = value * sign;

    if(value > INT_MAX)
        return INT_MAX;
    else if(value < INT_MIN)
        return INT_MIN;

    return value;
}