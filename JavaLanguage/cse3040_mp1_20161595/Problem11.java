package cse3040;

class PalindromeChecker{
	public static void check(String str) {
		int size=str.length();
		int flag=1;//if palindrome then 1, else 0;
		for(int i=0; i<size/2; i++)
		{
			if(str.charAt(i)!=str.charAt(size-i-1))//palindrome이 아닌 경우
			{
				flag=0;
				break;
			}
		}
		if(flag==0)
			System.out.println(str+" is not a palindrome.");
		else
			System.out.println(str+" is a palindrome.");
	}
	public static void check(int num)
	{
		String str=String.valueOf(num);
		int size=str.length();
		int flag=1;//if palindrome then 1, else 0;
		for(int i=0; i<size/2; i++)
		{
			if(str.charAt(i)!=str.charAt(size-i-1))//palindrome이 아닌 경우
			{
				flag=0;
				break;
			}
		}
		if(flag==0)
			System.out.println(num+" is not a palindrome.");
		else
			System.out.println(num+" is a palindrome.");
	}
}

public class Problem11 {
	public static void main(String[] args) {
	PalindromeChecker.check("abcde");
	PalindromeChecker.check("abcba");
	PalindromeChecker.check(1234);
	PalindromeChecker.check(12321);
	}
}