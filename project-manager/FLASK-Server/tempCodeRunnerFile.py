temp.append(str(serialString)[4:-1])
                    x += 1
                    if x > 5:
                        x = 0
                        y += 1
                    if y == 5:
                        print(len(temp))
                        #print(temp)
                        break