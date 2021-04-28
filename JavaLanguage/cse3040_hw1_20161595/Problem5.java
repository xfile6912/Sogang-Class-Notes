package cse3040;
import java.util.Scanner;

public class Problem5 {
	public static void main(String[] args)
	{
		int student[]= new int[5];
		int firstIndex=-1;
		int firstValue=-1;
		int secondIndex=-1;
		int secondValue=-1;
		
		Scanner reader=new Scanner(System.in);
	
		System.out.println("Enter exam scores of each student.");
		for(int i=0; i<5; i++)
		{
			System.out.printf("Score of student %d: ", i+1);
			student[i]=reader.nextInt();
		}
		for(int i=0; i<5; i++)
		{
			if(firstValue<student[i])
			{
				secondValue=firstValue;
				secondIndex=firstIndex;
				firstValue=student[i];
				firstIndex=i;
			}
			else if(secondValue<student[i])
			{
				secondValue=student[i];
				secondIndex=i;
			}
			
		}
		System.out.printf("The 1st place is student %d with %d points.\n", firstIndex+1, firstValue);
		System.out.printf("The 2nd place is student %d with %d points.\n", secondIndex+1, secondValue);
	}
	
}
