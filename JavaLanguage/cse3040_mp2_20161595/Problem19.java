package cse3040;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;

class BookInfo implements Comparable<BookInfo>{
	private int rank;
	private String name;
	private String author;
	private String price;
	public BookInfo(int rank, String name, String author, String price) {
		this.rank=rank;
		this.name=name;
		this.author=author;
		this.price=price;
	}
	@Override
	public int compareTo(BookInfo book) {
		if(this.rank<book.rank)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	public String toString()
	{
		return String.format("#%d %s, %s, %s", rank, name, author, price);
	}
}
class BookReader{
	public static ArrayList<BookInfo> readBooks(String site)
	{
		ArrayList<String> lines =new ArrayList<String>();
		ArrayList<BookInfo> books=new ArrayList<BookInfo>();
		URL url=null;
		BufferedReader reader=null;
		String line="";
		try {
			url=new URL(site);
			reader=new BufferedReader(new InputStreamReader(url.openStream()));
			while((line=reader.readLine())!=null)
			{
				if(line.trim().length()>0)
				{
					lines.add(line);
				}
			}
			reader.close();
		}catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		int rank=1;
		int status=0;
		String name="";
		String author="";
		String price="";
		for(int i=0; i<lines.size(); i++)
		{
			line=lines.get(i);
			
			if(status==0)
			{
				if(line.contains("div class=\"col-lg-9 product-info-listView\""))
				{
					status=1;
				}
			}
			else if(status==1)
			{
				if(line.contains("div class=\"product-shelf-title pr-m\""))
					status=2;
			}
			else if(status==2)
			{
				if(line.contains("a title=\"\" class=\" \""))
				{
					//System.out.println(line);
					int begin=line.indexOf("\">")+"\">".length();
					int end=line.indexOf("</a>");
					name=line.substring(begin,end);
					status=3;
				}
			}
			else if(status==3)
			{
				if(line.contains("div class=\"product-shelf-author contributors\""))
				{
					//System.out.println(line);
					int begin=line.indexOf("<a href=");
					line=line.substring(begin);//author는 a href부터 나오므로 line string을 <a href부터로 바꾸어줌.
					begin=line.indexOf("\">")+"\">".length();
					int end=line.indexOf("</a>");
					author=line.substring(begin,end);
					status=4;
				}
			}
			else if(status==4)
			{
				if(line.contains("div class=\"product-shelf-pricing mt-s mb-s\""))
				{
					status=5;
				}
			}
			else if(status==5)
			{
				if(line.contains("a title=\"\" class=\" current link\""))
				{
					//System.out.println(line);
					int begin=line.indexOf("\">")+"\">".length();
					int end=line.indexOf("</a>");
					price=line.substring(begin,end);
					BookInfo book=new BookInfo(rank, name, author, price);
					books.add(book);
					status=0;
					rank++;
				}
			}
		}
		return books;
	}
}
public class Problem19 {
	public static void main(String[] args) {
		ArrayList<BookInfo> books;
		books = BookReader.readBooks("https://www.barnesandnoble.com/b/books/_/N-1fZ29Z8q8");
		Collections.sort(books);
		for (int i = 0; i < books.size(); i++) {
			BookInfo book = books.get(i);
			System.out.println(book);
		}
	}
}
