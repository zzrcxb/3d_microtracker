#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "3d_tracker_socket.h"

using namespace std;

int split2double(const char* strin, vector<double> &data, char ch) {
    ;
    vector<char*>pos;
    double tempd;
    int count = 0;
    char* str = new char[strlen(strin) + 1];
    char* next = str;

    strcpy_s(str, strlen(strin) + 1, strin);

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ch) {
            pos.push_back(next);
            next = str + i + 1;
            str[i] = '\0';
        }
    }
    pos.push_back(next);

    for (int i = 0; i < pos.size(); i++) {
        if (fChanger(pos[i], tempd) != FAIL) {
            data.push_back(tempd);
        }
        else
            return FAIL;
    }

    delete[] str;

    return 0;
}


int intChanger(const char* str, int &res) {
    int i;
    int justtime = 1;

    res = 0;
    //Check parameter
    if (!str)
        return FAIL;

    //Convert
    for (i = 0; str[i] != '\0'; i++) {
        if (i == 0) {
            if (str[0] == '+' || str[0] == '-') {
                if (str[0] == '-')
                    justtime = -1;
                else
                    justtime = 1;
            }
            else {
                if (str[i] <= '9' && str[i] >= '0') {
                    res = str[i] - '0';
                }
                else
                    return FAIL;
            }
        }
        else {
            res = res * 10 + str[i] - '0';
        }
    }

    res *= justtime;

    return 0;
}

int fChanger(const char* str, double &res) {
    int i, j;
    int count = 0, pos, size;
    int b, a;
    char *before, *after;

    res = 0;
    //Check parameter
    if (!str)
        return FAIL;

    //Find '.'
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            pos = i;
            count++;
        }
    }
    size = i;
    //count == 0
    if (count == 0) {
        if (intChanger(str, b) == FAIL)
            return FAIL;
        res = (double)b;
        return 0;
    }
    //-.xxx
    if (str[0] == '-' && str[1] == '.') {
        after = new char[size - pos];
        if (!after)
            return FAIL;
        for (i = pos + 1, j = 0; i < size; i++, j++) {
            after[j] = str[i];
        }
        after[j] = '\0';

        if (intChanger(after, a) == FAIL)
            return FAIL;

        res = -(double)a / pow(10.0, (double)(size - pos - 1));

        return 0;
    }

    if (count != 1)
        return FAIL;
    if (pos + 1 == size)
        return FAIL;

    before = new char[pos];
    after = new char[size - pos];
    // cout << "There there 4" << endl;
    if (!before || !after)
        return FAIL;

    for (i = 0, j = 0; i < pos; i++, j++) {
        before[j] = str[i];
    }
    before[j] = '\0';

    for (i = pos + 1, j = 0; i < size; i++, j++) {
        after[j] = str[i];
    }
    after[j] = '\0';
    //cout << "There there 5" << endl;
    if (intChanger(before, b) == FAIL)
        return FAIL;
    if (intChanger(after, a) == FAIL)
        return FAIL;
    //Combine;
    //cout << "There there 6" << endl;
    res = (double)b + (double)a / pow(10.0, (double)(size - pos - 1));
    return 0;
}

int charChanger(double num, char* &res) {
    int i = 0, j = 0, temp = 0;
    int size = 1, div = 10;
    int before, intafter;
    double after;
    int aftersize = 0, beforesize = 0;
    //
    res = NULL;
    //Get Size;
    after = num - (int)num;
    before = (int)num;
    after = fabs(after);
    before = abs(before);

    for (i = 1; before >= 10; i++) {
        before /= 10;
    }
    if (after) {
        for (j = 0; after < 1; j++) {
            after *= 10;
        }
        j++;
    }
    after = num - (int)num;
    before = (int)num;
    after = fabs(after);
    before = abs(before);

    intafter = after * pow(10, j - 1);
    beforesize = i;
    aftersize = j - 1;


    if (num < 0)
        j++;
    size = i + j + 1;
    //Assign memory
    res = new char[size];
    if (!res)
        return -1;
    //Give value
    i = 0;
    if (num < 0)
        res[i++] = '-';
    for (j = beforesize - 1; i < size && j >= 0; i++, j--) {
        temp = before / (int)pow(10, j);
        res[i] = temp + '0';
        before = before % (int)pow(10, j);
    }
    if (after) {
        res[i++] = '.';
        for (j = aftersize - 1; i < size && j >= 0; i++, j--) {
            temp = intafter / (int)pow(10, j);
            res[i] = temp + '0';
            intafter = intafter % (int)pow(10, j);
        }
    }
    res[i] = '\0';

    return size;
}