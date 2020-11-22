import cv2


def process_without_ML(image):
    img = cv2.imread(r'{}'.format(image), 0)
    cv2.imshow('image', img)
    ans = []
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[40 + (i * 4):40 + (i * 4) + 1,
                    20 + (i * 4):20 + (i * 4) + 1].mean()))
    ans.append(tmp)
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[40 + (i * 4):40 + (i * 4) + 1,
                    70 + (i * 4):70 + (i * 4) + 1].mean()))
    ans.append(tmp)
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[100 + (i * 4):100 + (i * 4) + 1,
                    20 + (i * 4):20 + (i * 4) + 1].mean()))
    ans.append(tmp)
    tmp = []
    for i in range(4):
        tmp.append(
            int(img[100 + (i * 4):100 + (i * 4) + 1,
                    70 + (i * 4):70 + (i * 4) + 1].mean()))
    ans.append(tmp)
    pattern = ""
    for x in ans:
        avg = int(sum(x) / len(x))
        x.append(avg)
        pattern += '0' if avg < 70 else '1'
    print(ans, pattern)
    return ans, pattern


#ans, pattern = process_without_ML('./{}.bmp'.format('M'))