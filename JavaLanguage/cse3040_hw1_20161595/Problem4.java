package cse3040;
import java.util.Scanner;

public class Problem4 {
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String text;
		String str;
		int result=0;
		Scanner reader=new Scanner(System.in);
		
		System.out.print("Enter a text: ");
		text=reader.nextLine();
		while(true)
		{
			System.out.print("Enter a string: ");
			str=reader.nextLine();
			if(str.length()==0)
			{
				System.out.println("You must enter a string.");
			}
			else
			{
				break;
			}
				
		}
		for(int i=0; i<text.length(); i++)
		{
			int flag=1;
			if(text.charAt(i)==str.charAt(0))
			{
				for(int j=1; j<str.length(); j++)
				{
					if(i+j>=text.length() || text.charAt(i+j)!=str.charAt(j) )//not matched
					{
						flag=0;
						break;
					}
				}
				if(flag==1)
				{
					result++;
				}
			}
		}
		System.out.printf("There are %d instances of \"%s\".\n", result, str);
	}
}
