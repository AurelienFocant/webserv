const DIRECTORY = "/upload/";

function loadFiles() {
	fetch(DIRECTORY)
		.then(res => res.text())
		.then(htmlText => {
			const parser = new DOMParser();
			const doc = parser.parseFromString(htmlText, "text/html");

			const links = Array.from(doc.querySelectorAll("a"));
			const ul = document.getElementById("fileList");
			ul.innerHTML = "";

			if (links.length === 0) {
				ul.innerHTML = "<li class='loading'>No files found</li>";
				return;
			}

			links.forEach(link => {
				const href = link.getAttribute("href");
				if (href === "../") return;

				const li = document.createElement("li");
				li.textContent = href;

				const div  = document.createElement("div");
				div.className = "actions";

				const btnDl = document.createElement("a");
				btnDl.href = DIRECTORY + href;
				btnDl.textContent = "Download";
				btnDl.className = "button";

				const btnDel = document.createElement("button");
				btnDel.textContent = "Delete";
				btnDel.className = "button";
				btnDel.onclick = () => deleteFile(href);

				div.appendChild(btnDl);
				div.appendChild(btnDel);
				li.appendChild(div);
				ul.appendChild(li);
			});
		})
		.catch(err => {
			console.error("Error loading files:", err);
			const ul = document.getElementById("fileList");
			ul.innerHTML = "<li class='loading'>Error loading files</li>";
		});
}

function deleteFile(filename) {
	if (!confirm(`Delete "${filename}"?`)) return;

	fetch(DIRECTORY + filename, { method: "DELETE" })
		.then(res => {
			if (!res.ok) throw new Error("Delete failed");
			alert(`Deleted ${filename}`);
			loadFiles();
		})
		.catch(err => {
			console.error(err);
			alert("Error deleting file");
		});
}

loadFiles();

const form = document.getElementById("uploadForm");

form.addEventListener("submit", function (e) {
    e.preventDefault(); // stop normal form submission

    const formData = new FormData(form);

    fetch("/upload/", {
        method: "POST",
        body: formData
    })
    .then(res => {
        if (res.status === 201) {
            // ✅ Reload page after successful creation
            window.location.reload();
        } else {
            throw new Error("Upload failed");
        }
    })
    .catch(err => {
        console.error(err);
        alert("Error uploading file");
    });
});

