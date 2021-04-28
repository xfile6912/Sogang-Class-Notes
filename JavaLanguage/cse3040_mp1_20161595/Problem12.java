package cse3040;

class SubsequenceChecker{
	public static void check(String str, String substr)
	{
		int j=0;
		int []index= new int[substr.length()];//index 저장
		for(int i=0; i<str.length(); i++)
		{
			if(j<substr.length() && str.charAt(i)==substr.charAt(j))//j가 substr.length()와 같아지면 range벗어나는 값 체크하는 것이므로
			{
				index[j]=i;
				j++;
			}
		}
		if(j==substr.length())//substr이라면
		{
			System.out.println(substr+" is a subsequence of "+ str);
			for(int i=0; i<substr.length(); i++)
			{
				System.out.print(index[i]+" ");
			}
			System.out.println();
		}
		else
		{
			System.out.println(substr+" is not a subsequence of "+ str);
		}
	}
}

public class Problem12 {
	public static void main(String[] args) {
		SubsequenceChecker.check("supercalifragilisticexpialidocious", "pads");
		SubsequenceChecker.check("supercalifragilisticexpialidocious", "padsx");
	}
}
