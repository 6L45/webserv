import tkinter as tk

# Create the main window
window = tk.Tk()
window.title("HTTP Header Builder")
empty_frame = tk.Frame(window, height=0, width=0)
# Create the widgets


# Create a variable to store the selected option
selected_option = tk.StringVar()

# Create the radiobuttons
option1 = tk.Radiobutton(window, text="GET", variable=selected_option, value="GET")
option2 = tk.Radiobutton(window, text="POST", variable=selected_option, value="POST")
option3 = tk.Radiobutton(window, text="DEL", variable=selected_option, value="DELETE")


# COMMON ALL METHOD --------------------------------------------------

url_label = tk.Label(text="URL:")
url_entry = tk.Entry()

host_label = tk.Label(text="Host:")
host_entry = tk.Entry()

body_label = tk.Label(text="Body:")
body_text = tk.Text()

charset_label = tk.Label(text="Accept-Charset")
charset_entry = tk.Entry()

accept_label = tk.Label(text="Accept")
accept_entry = tk.Entry()

encoding_label = tk.Label(text="Accept-Encoding")
encoding_entry = tk.Entry()

language_label = tk.Label(text="Accept-Language")
language_entry = tk.Entry()

cache_label = tk.Label(text="Cache-Control")
cache_entry = tk.Entry()

connection_label = tk.Label(text="Connection")
connection_entry = tk.Entry()

cookie_label = tk.Label(text="Cookie")
cookie_entry = tk.Entry()
# ----------------------------------------------------------------

# FOR SPECIFIC METHOD ---------------------------------------------
method_var1_label = tk.Label(text="")
method_var1_entry = tk.Entry()

method_var2_label = tk.Label(text="")
method_var2_entry = tk.Entry()
# ----------------------------------------------------------------

# Set the default values for the widgets
url_entry.insert(0, "localhost:18002")
host_entry.insert(0, url_entry.get())
charset_entry.insert(0, "utf-8")

# Define the callback function for the button
def build_header():
    # Build the HTTP header
    method = selected_option.get().strip()
    url = url_entry.get().strip()
    host = host_entry.get().strip()
    charset = charset_entry.get().strip()
    accept = accept_entry.get().strip()
    encoding = encoding_entry.get().strip()
    language = language_entry.get().strip()
    cache = cache_entry.get().strip()
    connection = connection_entry.get().strip()
    cookie = cookie_entry.get().strip()
    var1 = method_var1_entry.get().strip()
    var2 = method_var2_entry.get().strip()

    body = body_text.get("1.0", "end").strip()

    request = f"{method} {url} HTTP/1.1\r\n"

    if len(host):
        request += f"Host: {host}\r\n"

    if len(charset):
        request += f"Accept-Charset: {charset}\r\n"

    if len(accept):
        request += f"Accept: {accept}\r\n"

    if len(encoding):
        request += f"Accept-Encoding: {encoding}\r\n"

    if len(language):
        request += f"Accept-Language: {language}\r\n"

    if len(cache):
        request += f"Cache-Control: {cache}\r\n"

    if len(connection):
        request += f"Connection: {connection}\r\n"

    if len(cookie):
        request += f"Cookie: {cookie}\r\n"

    if method == "GET":
        if len(var1):
            request += f"If-Modified-Since: {var1}\r\n"
        if len(var2):
            request += f"Referer: {var2}\r\n"

    elif method == "POST":
        if len(var1):
            request += f"Content-Length: {var1}\r\n"
        if len(var2):
            request += f"Content-Type: {var2}\r\n"

    elif method == "DELETE":
        if len(var1):
            request += f"If-Match: {var1}\r\n"
        if len(var2):
            request += f"If-Unmodified-Since: {var2}\r\n"

    request += "\r\n"

    if len(body):
        request += body + "\r\n\r\n"

    print(request, end="")



def update_entry(name, index, mode):
    # Get the value of the selected option
    option = selected_option.get()
    
    # Update the Entry widget based on the selected option
    if option == "GET":
        method_var1_label.configure(text="If-Modified-Since")
        method_var2_label.configure(text="Referer")
    elif option == "POST":
        method_var1_label.configure(text="Content-Length")
        method_var2_label.configure(text="Content-Type")
    elif option == "DELETE":
        method_var1_label.configure(text="If-Match")
        method_var2_label.configure(text="If-Unmodified-Since")

# Set up a trace to call the update_entry function whenever the value of the selected_option variable changes
selected_option.trace("w", update_entry)

# Create the button
button = tk.Button(text="Build Request", command=build_header)

# Create the layout
option1.grid(row=0, column=0)
option2.grid(row=0, column=1)
option3.grid(row=0, column=2)

url_label.grid(row=1, column=0)
url_entry.grid(row=1, column=1)

host_label.grid(row=2, column=0)
host_entry.grid(row=2, column=1)

charset_label.grid(row=3, column=0)
charset_entry.grid(row=3, column=1)

accept_label.grid(row=4, column=0)
accept_entry.grid(row=4, column=1)

encoding_label.grid(row=5, column=0)
encoding_entry.grid(row=5, column=1)

language_label.grid(row=6, column=0)
language_entry.grid(row=6, column=1)

cache_label.grid(row=7, column=0)
cache_entry.grid(row=7, column=1)

connection_label.grid(row=8, column=0)
connection_entry.grid(row=8, column=1)

cookie_label.grid(row=9, column=0)
cookie_entry.grid(row=9, column=1)

method_var1_label.grid(row=10, column=0)
method_var1_entry.grid(row=10, column=1)

method_var2_label.grid(row=11, column=0)
method_var2_entry.grid(row=11, column=1)

body_label.grid(row=12, column=0)
body_text.grid(row=12, column=1)

button.grid(row=13, column=1)

# Run the main loop
window.mainloop()

