x = [[70, 79, 81, 83, 78], [0, 70, 72, 70, 68], [74, 73, 71, 70, 72],
     [72, 70, 67, 64, 68]]


def encode_dec_to_data(arr):
    tmp = ''
    arr = arr[0] + arr[1] + arr[2] + arr[3]
    #print(arr)
    for x in arr:
        #print(x, chr(x))
        tmp += str(chr(x))
    #print(len(tmp))
    return '*' + tmp + '*'


def decode_dec_to_data(code):
    postition = [[40, 20], [40, 70], [100, 20], [100, 70]]
    tmp = ''
    tmp += ("*** Answer from PC2 ***")
    if code[0] == '*' and code[-1] == '*' and len(code) == 22:
        idx = 0
        idy = 0
        for x in code[1:-1]:
            if idx < 4:
                tmp += (('(' + str(postition[idy][0] + (idx * 4)) + ',' +
                         str(postition[idy][1] +
                             (idx * 4)) + '): ' + str(ord(x)))) + '\n'
            else:
                tmp += (
                    ('mean of QUADRANT {} :'.format(idy) + str(ord(x)))) + '\n'
            idx += 1
            if idx == 5:
                idx = 0
                idy += 1
    return tmp[:-1] + '\n' + "*** --------------- ***"


print(encode_dec_to_data(x))
print(decode_dec_to_data(encode_dec_to_data(x)))