package cse3040fp;


import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.TreeSet;

class Book implements Comparable<Book> {
	private String author;
	private String title;
	private String borrower;

	Book(String title) {
		this.title = title;
		this.borrower = "-";
	}

	Book(String title, String author) {
		this.title = title;
		this.author = author;
		this.borrower = "-";
	}

	Book(String title, String author, String borrower) {
		this.title = title;
		this.author = author;
		this.borrower = borrower;
	}

	public String getAuthor() {
		return author;
	}

	public void setAuthor(String author) {
		this.author = author;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public String getBorrower() {
		return borrower;
	}

	public void setBorrower(String borrower) {
		this.borrower = borrower;
	}

	public int hashCode() {
		String temp = "";
		for (int i = 0; i < this.title.length(); i++) {
			if ('a' <= this.title.charAt(i) && 'z' >= this.title.charAt(i))
				temp += (char) (this.title.charAt(i) - ('a' - 'A'));
			else
				temp += (char) this.title.charAt(i);
		}
		return temp.hashCode();
	}

	public boolean equals(Book other) {
		if (this.title.equalsIgnoreCase(other.getTitle()))
			return true;
		return false;
	}

	@Override
	public int compareTo(Book other) {
		return this.title.compareToIgnoreCase(other.getTitle());
	}

}

class Manage {
	TreeSet<Book> library;

	Manage(TreeSet<Book> library) {
		this.library = library;
	}

	public synchronized void updateFile() {
		PrintWriter pw;
		try {
			pw = new PrintWriter("books.txt");
			for (Book book : library)// 책 찾아줌.
			{
				pw.println(book.getTitle() + "\t" + book.getAuthor() + "\t" + book.getBorrower());
			}
			pw.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void getBooksFromFile() {
		BufferedReader br;
		try {
			br = new BufferedReader(new FileReader("books.txt"));
			while (true) {
				String line = br.readLine();
				if (line == null)
					break;
				String[] temp = line.split("\t");
				String title = temp[0];
				String author = temp[1];
				String borrower = temp[2];
				Book book = new Book(title, author, borrower);
				library.add(book);
			}
		} catch (FileNotFoundException e) {
			try {
				PrintWriter pw = new PrintWriter("books.txt");
			} catch (FileNotFoundException e1) {
			}
		} catch (IOException e) {
		}
	}
}

public class Server {
	TreeSet<Book> library;
	ServerSocket serverSocket;
	int portNumber;
	Socket socket;
	Manage manage;

	Server(int portNumber) {
		this.portNumber = portNumber;
		library = new TreeSet<Book>();
		Collections.synchronizedSet(library);
		manage = new Manage(library);
	}

	public void start() {
		try {
			serverSocket = new ServerSocket(portNumber);
			manage.getBooksFromFile();
			while (true) {
				socket = serverSocket.accept();
				ServerThread thread = new ServerThread(socket, manage);
				thread.start();
			}
		} catch (IOException e) {
			System.exit(0);
		}
	}

	public static void main(String[] args) {
		int portNumber;
		if (args.length != 1) {
			System.out.println("Please give the port number as an argument.");
			System.exit(0);
		}
		portNumber = Integer.parseInt(args[0]);
		new Server(portNumber).start();
	}

	class ServerThread extends Thread {
		Socket socket;
		DataInputStream in;
		DataOutputStream out;
		Manage manage;

		ServerThread(Socket socket, Manage manage) {
			this.socket = socket;
			this.manage = manage;
			try {
				in = new DataInputStream(socket.getInputStream());
				out = new DataOutputStream(socket.getOutputStream());
			} catch (IOException e) {
			}

		}

		public void run() {
			String order;
			try {
				while (in != null) {
					order = in.readUTF();
					if (order.equals("add"))
						dealAdd();
					else if (order.equals("borrow"))
						dealBorrow();
					else if (order.equals("return"))
						dealReturn();
					else if (order.equals("info"))
						dealInfo();
					else if (order.equals("search"))
						dealSearch();
					manage.updateFile();
				}
			} catch (IOException e) {
			}
		}

		private void dealAdd() {
			String title;
			String author;
			try {
				title = in.readUTF();
				author = in.readUTF();
				Book book = new Book(title, author);
				if (library.contains(book)) {
					out.writeUTF("exist");
				} else {
					library.add(book);
					out.writeUTF("succes");
				}
			} catch (IOException e) {
			}
		}

		private void dealBorrow() {
			String title;
			String borrower;
			try {
				borrower = in.readUTF();
				title = in.readUTF();
				Book book = new Book(title);
				if (!library.contains(book))// 도서관에 책이 없다면
				{
					out.writeUTF("not available");
					return;
				}
				for (Book temp : library)// 책 찾아줌.
				{
					if (temp.equals(book)) {
						book = temp;
						break;
					}
				}
				if (!book.getBorrower().equals("-")) {
					out.writeUTF("not available");
					return;
				} else {
					book.setBorrower(borrower);
					out.writeUTF("success");// 최종 성공한 경우
					out.writeUTF(book.getTitle());// 책 제목 보내줌.
				}
			} catch (IOException e) {
			}
		}

		private void dealReturn() {
			String title;
			String userID;
			try {
				userID = in.readUTF();
				title = in.readUTF();
				Book book = new Book(title);
				if (!library.contains(book))// 도서관에 책이 없다면
				{
					out.writeUTF("not available");
					return;
				}
				for (Book temp : library)// 책 찾아줌.
				{
					if (temp.equals(book)) {
						book = temp;
						break;
					}
				}
				if (!book.getBorrower().equals(userID)) {
					out.writeUTF("not available");
					return;
				} else {
					book.setBorrower("-");
					out.writeUTF("success");// 최종 성공한 경우
					out.writeUTF(book.getTitle());
				}
			} catch (IOException e) {
			}
		}

		private void dealInfo() {
			String title;
			String userID;
			try {
				userID = in.readUTF();
				ArrayList<Book> borrowList = new ArrayList<>();
				for (Book book : library)// 책 찾아줌.
				{
					if (book.getBorrower().equals(userID)) {
						borrowList.add(book);
					}
				}
				String answer = "You are currently borrowing " + borrowList.size() + " books:\n";
				for (int i = 0; i < borrowList.size(); i++) {
					Book book = borrowList.get(i);
					answer += (i + 1) + ". " + book.getTitle() + ", " + book.getAuthor() + "\n";
				}
				out.writeUTF(answer);
			} catch (IOException e) {
			}
		}

		private void dealSearch() {
			String search;
			try {
				search = in.readUTF();
				ArrayList<Book> searchList = new ArrayList<>();
				for (Book book : library)// 책 찾아줌.
				{
					if (book.getTitle().toLowerCase().contains(search.toLowerCase()))
						searchList.add(book);
					else if (book.getAuthor().toLowerCase().contains(search.toLowerCase()))
						searchList.add(book);
				}
				String answer = "Your search matched " + searchList.size() + " results.\n";
				for (int i = 0; i < searchList.size(); i++) {
					Book book = searchList.get(i);
					answer += (i + 1) + ". " + book.getTitle() + ", " + book.getAuthor() + "\n";
				}
				out.writeUTF(answer);
			} catch (IOException e) {
			}
		}
	}
}

