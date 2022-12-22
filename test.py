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

body_label = tk.Label(text="Body:")
body_text = tk.Text()

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
    body = body_text.get("1.0", "end")

    request = f"{method} {url} HTTP/1.1\r\n\
Host: {host}\r\n\
Accept_Charset: {charset}\r\n\
\r\n"

    if len(body) == 0:
        request += f"{body}\r\n\
\r\n"

    # Print the HTTP header
    print(request, end="")

# Create the button
button = tk.Button(text="Build Request", command=build_header)

# Create the layout
method_label.grid(row=0, column=0)
method_entry.grid(row=0, column=1)

url_label.grid(row=1, column=0)
url_entry.grid(row=1, column=1)

host_label.grid(row=2, column=0)
host_entry.grid(row=2, column=1)

charset_label.grid(row=3, column=0)
charset_entry.grid(row=3, column=1)

body_label.grid(row=4, column=0)
body_text.grid(row=4, column=1)

button.grid(row=5, column=1)

# Run the main loop
window.mainloop()


