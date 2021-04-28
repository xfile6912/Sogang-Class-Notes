package cse3040;
import java.util.Scanner;

public class Problem1 {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String letter;	
		
		System.out.print("ASCII code teller. Enter a letter: ");
		Scanner reader=new Scanner(System.in);
		letter=reader.nextLine();
		if(letter.length()==1)
		{
			char c= letter.charAt(0);
			if((c<='z' && c>='a') || (c<='Z' && c>='A'))
			{
				System.out.printf("The ASCII code of %c is %d.", c, (int)c);
			}
			else
			{
				System.out.println("You must input a single uppercase or lowercase alphabet.");
			}
		}
		else
		{
			System.out.println("You must input a single uppercase or lowercase alphabet.");
		}
	}

}
