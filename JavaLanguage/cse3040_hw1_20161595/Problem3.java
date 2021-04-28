package cse3040;
import java.util.Scanner;

public class Problem3 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String text;
		String temp;
		char letter;
		int result=0;
		Scanner reader=new Scanner(System.in);
		
		System.out.print("Enter a text: ");
		text=reader.nextLine();
		while(true)
		{
			System.out.print("Enter a letter: ");
			temp=reader.nextLine();
			if(temp.length()==1)
			{
				letter=temp.charAt(0);
				break;
			}
			else
				System.out.println("You must enter a single letter.");
		}
		for(int i=0; i<text.length(); i++)
		{
			if(text.charAt(i)==letter)
			{
				result++;
			}
		}
		System.out.printf("There are %d %c's in the text.\n", result, letter);
	}

}
