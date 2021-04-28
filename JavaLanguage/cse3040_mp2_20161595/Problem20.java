package mp2_20;

import java.util.ArrayList;
import java.util.Collections;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.select.Elements;

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
	public static ArrayList<BookInfo> readBooksJsoup(String site)
	{
		ArrayList<BookInfo> books=new ArrayList<BookInfo>();
		Document doc=null;
		try {
			doc=Jsoup.connect(site).get();
		}catch(Exception e)
		{
			e.printStackTrace();
		}
		Elements bestsellersArea=doc.select("div.col-lg-9");
		Elements titleArea=bestsellersArea.select("div.product-shelf-title");
		Elements titles=titleArea.select("a[title]");
		Elements authors=bestsellersArea.select("div.product-shelf-author");
		Elements priceArea=bestsellersArea.select("div.product-shelf-pricing");
		Elements priceArea2=priceArea.select("td.buy-new");
		Elements prices=priceArea2.select("a[title]");
		for(int i=0; i<titles.size(); i++)
		{
			String author=authors.eq(i).text();
			int begin=author.indexOf("by ")+"by ".length();
			int end=author.indexOf(",");
			if(end!=-1)
			{
				author=author.substring(begin,end);
			}
			else
				author=author.substring(begin);
			books.add(new BookInfo(i+1, titles.eq(i).text(), author, prices.eq(i).text()));
		}
		return books;
	}
}
public class Problem20 {
	public static void main(String[] args) {
		ArrayList<BookInfo> books;
		books = BookReader.readBooksJsoup("https://www.barnesandnoble.com/b/books/_/N-1fZ29Z8q8");
		Collections.sort(books);
		for (int i = 0; i < books.size(); i++) {
			BookInfo book = books.get(i);
			System.out.println(book);
		}
	}
}
