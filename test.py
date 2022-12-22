"""
import tkinter as tk

# Create the main window
window = tk.Tk()
window.title("HTTP Header Builder")

# Create the widgets
method_label = tk.Label(text="Method:")
method_entry = tk.Entry()

url_label = tk.Label(text="URL:")
url_entry = tk.Entry()

header_label = tk.Label(text="Header:")
header_text = tk.Text()

# Set the default values for the widgets
method_entry.insert(0, "GET")
url_entry.insert(0, "http://example.com/")

# Define the callback function for the button
def build_header():
    # Build the HTTP header
    method = method_entry.get()
    url = url_entry.get()
    header = header_text.get("1.0", "end")

    request = f"{method} {url} HTTP/1.1\r\n{header}\r\n\r\n"

    # Print the HTTP header
    print(request)

# Create the button
button = tk.Button(text="Build Header", command=build_header)

# Create the layout
method_label.pack()
method_entry.pack()

url_label.pack()
url_entry.pack()

header_label.pack()
header_text.pack()

button.pack()

# Run the main loop
window.mainloop()
"""
import tkinter as tk

# Create the main window
window = tk.Tk()
window.title("HTTP Header Builder")

# Create the widgets
method_label = tk.Label(text="Method:")
method_entry = tk.Entry()

url_label = tk.Label(text="URL:")
url_entry = tk.Entry()

host_label = tk.Label(text="Host:")
host_entry = tk.Entry()

header_label = tk.Label(text="Body:")
header_text = tk.Text()

charset_label = tk.Label(text="Accept-Charset")
charset_entry = tk.Entry()

# Set the default values for the widgets
method_entry.insert(0, "GET")
url_entry.insert(0, "localhost:18002")
host_entry.insert(0, url_entry.get())
charset_entry.insert(0, "utf-8")

# Define the callback function for the button
def build_header():
    # Build the HTTP header
    method = method_entry.get()
    url = url_entry.get()
    host = host_entry.get()
    charset = charset_entry.get()
    header = header_text.get("1.0", "end")

    request = f"{method} {url} HTTP/1.1\r\nHost: {host}\r\nAccept_Charset: {charset}\r\n\r\n{header}\r\n\r\n"

    # Print the HTTP header
    print(request)

# Create the button
button = tk.Button(text="Build Header", command=build_header)

# Create the layout
method_label.grid(row=0, column=0)
method_entry.grid(row=0, column=1)

url_label.grid(row=1, column=0)
url_entry.grid(row=1, column=1)

host_label.grid(row=2, column=0)
host_entry.grid(row=2, column=1)

charset_label.grid(row=3, column=0)
charset_entry.grid(row=3, column=1)

header_label.grid(row=4, column=0)
header_text.grid(row=4, column=1)

button.grid(row=5, column=1)

# Run the main loop
window.mainloop()


