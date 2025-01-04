class Solution(object):

# Method 1 Brute Force
    # def twoSum(self, nums, target):
    #     """
    #     :type nums: List[int]
    #     :type target: int
    #     :rtype: List[int]
    #     """
        
    #     for index, num in enumerate(nums):
    #         for index2, second in enumerate(nums[index + 1:len(nums)], index + 1):
    #             if num + second == target:
    #                 return [index, index2]


#####  Method 2 Hash Map

# need to use dictionaries. they are like built-in hash maps

# example key: value
# thisdict = {
#   num1: [index1, 2, 3...],
#   num2: [index1, 2, 3...],
#   num3: [index1, 2, 3...],
#   "colors": ["red", "white", "blue"]
# }

    def twoSum(self, nums, target):
    #     """
    #     :type nums: List[int]
    #     :type target: int
    #     :rtype: List[int]
    #     """

        hashmap = {}
        # hashmap = dict((num, hashmap[num].append(index)) for index, num in enumerate(nums)) 
        # hashmap = dict((num, index) for index, num in enumerate(nums)) # dictionary comprehension doesnt work well when mutating or accumulating (appending). 
        # Create hashmap
        for index, num in enumerate(nums):
            if num in hashmap: # if a duplicate exists append it to the dictionary
                hashmap[num].append(index)
            else:
                hashmap[num] = [index]

        for index, num in enumerate(nums):
            if target - num == num and len(hashmap[num]) == 2: # Case dupes = target
                return [hashmap[num][0], hashmap[num][1]]
            elif target - num == num and len(hashmap[num]) == 1 \
                                      or len(hashmap[num]) > 3:
                continue # Skip if 2x num = target while no dupes or to many dupes
            elif target - num in hashmap: # Otherwise look for second num
                return [index, hashmap[target - num][0]]