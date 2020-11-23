import Augmentor

# p = Augmentor.Pipeline("../images/capture/train/{}/".format("0001"))
# p.ground_truth("../images/capture/train{}/".format("0001"))
# p.rotate(probability=1, max_left_rotation=3, max_right_rotation=3)
# p.zoom_random(probability=1, percentage_area=0.9)
# p.sample(20)

# p = Augmentor.Pipeline("../images/capture/validation/{}/".format("0001"))
# p.ground_truth("../images/capture/validation{}/".format("0001"))
# p.rotate(probability=1, max_left_rotation=3, max_right_rotation=3)
# p.zoom_random(probability=1, percentage_area=0.9)
# p.sample(5)

for x in ["0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010","1011", "1100", "1101", "1110", "1111"]:
    p = Augmentor.Pipeline("../images/capture/train/{}/".format(x))
    p.ground_truth("../images/capture/train/{}/".format(x))
    p.rotate(probability=1, max_left_rotation=3, max_right_rotation=3)
    p.zoom_random(probability=1, percentage_area=0.9)
    p.sample(20)
    a = Augmentor.Pipeline("../images/capture/validation/{}/".format(x))
    a.ground_truth("../images/capture/validation/{}/".format(x))
    a.rotate(probability=1, max_left_rotation=3, max_right_rotation=3)
    a.zoom_random(probability=1, percentage_area=0.9)
    a.sample(4)