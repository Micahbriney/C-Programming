class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:

        substring = ''
        longestLength = 0

        for char in s:
            if(char in substring):
                substring = substring.split(char)[1]
                substring += char
            else:
                substring += char

            if(longestLength < len(substring)):
                longestLength = len(substring)

        return longestLength
