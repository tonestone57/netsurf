/*
 * Generics for Duktape binding in NetSurf
 *
 * The result of this *MUST* be setting a NetSurf object only.
 *
 * That object will then be absorbed into the global object as a hidden
 * object which is used by the rest of the bindings.
 */

var NetSurf = {
    /* The make-proxy call for list-type objects */
    makeListProxy: function(inner) {
	return new Proxy(inner, {
	    has: function(target, key) {
		if (typeof key == 'number') {
		    return (key >= 0) && (key < target.length);
		} else {
		    return key in target;
		}
	    },
	    get: function(target, key) {
		if (typeof key == 'number') {
		    if (typeof target.item === 'function') {
			return target.item(key);
		    }
		    return target[key];
		} else {
		    return target[key];
		}
	    },
	});
    },
    /* The make-proxy call for nodemap-type objects */
    makeNodeMapProxy: function(inner) {
	return new Proxy(inner, {
	    has: function(target, key) {
		if (typeof key == 'number') {
		    return (key >= 0) && (key < target.length);
		} else {
		    return target.getNamedItem(key) || (key in target);
		}
	    },
	    get: function(target, key) {
		if (typeof key == 'number') {
		    return target.item(key);
		} else {
		    var attr = target.getNamedItem(key);
		    if (attr) {
			return attr;
		    }
		    return target[key];
		}
	    },
	});
    },
    consoleFormatter: function Formatter() {

	if (arguments.length == 0) {
	    return new Array("");
	} else if (arguments.length == 1) {
	    return new Array(arguments[0].toString());
	}

	const target = arguments[0];
	const current = arguments[1];

	if (typeof target !== "string") {
	    return Array.from(arguments);
	}

	const offset = target.search("%");

	if (offset == -1 || offset >= (target.length - 1)) {
	    // We've a string, but the % either doesn't exist or is
	    // at the end of it, so give up
	    return Array.from(arguments);
	}

	const specifier = target[offset + 1];

	var converted = undefined;

	if (specifier === 's') {
	    // Stringification
	    converted = current.toString();
	} else if (specifier === 'd' || specifier === 'i') {
	    converted = parseInt(current, 10).toString();
	} else if (specifier === 'f') {
	    converted = parseFloat(current).toString();
	} else if (specifier === 'o') {
	    // TODO: Objectification?
	    converted = current.toString();
	} else if (specifier === 'O') {
	    // TODO: JSONification
	    converted = current.toString();
	}

	var result = new Array();

	if (converted !== undefined) {
	    // We converted it, so we need to absorb the formatted thing
	    // and move on
	    var newtarget = "";
	    if (offset > 0) {
		newtarget = target.substring(0, offset);
	    }
	    newtarget = newtarget + converted;
	    if (offset < target.length - 2) {
		newtarget = newtarget + target.substring(offset + 2, target.length);
	    }
	    result.push(newtarget);
	} else {
	    // Undefined, so we drop this argument and move on
	    result.push(target);
	}

	var i;
	for (i = 2; i < arguments.length; i++) {
	    result.push(arguments[i]);
	}

	if (result[0].search("%") == -1) {
	    return result;
	}

	if (result.length === 1) {
	    return result;
	}

	return Formatter.apply(result);
    },
    getElementsByClassName: function(root, classNames) {
        var names = classNames.split(/\s+/).filter(Boolean);
        if (names.length === 0) {
            return {
                length: 0,
                item: function() { return null; },
                toString: function() { return "[object HTMLCollection]"; }
            };
        }
        function performSearch() {
            var res = [];
            function walk(node) {
                var child = node.firstChild;
                while (child) {
                    if (child.nodeType === 1) { // ELEMENT_NODE
                        var classList = child.classList;
                        if (classList) {
                            var matches = names.every(function(name) {
                                return classList.contains(name);
                            });
                            if (matches) {
                                res.push(child);
                            }
                        }
                        walk(child);
                    }
                    child = child.nextSibling;
                }
            }
            walk(root);
            return res;
        }
        /* Returns a "live" collection object */
        return {
            get length() { return performSearch().length; },
            item: function(idx) { return performSearch()[idx]; },
            toString: function() { return "[object HTMLCollection]"; }
        };
    },
    makeDatasetProxy: function(element) {
        return new Proxy({}, {
            get: function(target, name) {
                var attrName = "data-" + name.replace(/([A-Z])/g, "-$1").toLowerCase();
                return element.getAttribute(attrName);
            },
            set: function(target, name, value) {
                var attrName = "data-" + name.replace(/([A-Z])/g, "-$1").toLowerCase();
                element.setAttribute(attrName, value);
                return true;
            },
            deleteProperty: function(target, name) {
                var attrName = "data-" + name.replace(/([A-Z])/g, "-$1").toLowerCase();
                element.removeAttribute(attrName);
                return true;
            },
            ownKeys: function(target) {
                var keys = [];
                var attrs = element.attributes;
                for (var i = 0; i < attrs.length; i++) {
                    var name = attrs[i].name;
                    if (name.startsWith("data-")) {
                        var camel = name.slice(5).replace(/-([a-z])/g, function(g) { return g[1].toUpperCase(); });
                        keys.push(camel);
                    }
                }
                return keys;
            },
            getOwnPropertyDescriptor: function(target, name) {
                var value = this.get(target, name);
                if (value !== null) {
                    return { value: value, enumerable: true, configurable: true, writable: true };
                }
            }
        });
    },
    getChildren: function(root) {
        var result = [];
        var child = root.firstChild;
        while (child) {
            if (child.nodeType === 1) { // ELEMENT_NODE
                result.push(child);
            }
            child = child.nextSibling;
        }
        return result;
    },
    mutationHelper: function(args, ownerDocument) {
        if (args.length === 1) {
            var arg = args[0];
            if (typeof arg === 'string') {
                return ownerDocument.createTextNode(arg);
            }
            return arg;
        }
        var fragment = ownerDocument.createDocumentFragment();
        for (var i = 0; i < args.length; i++) {
            var arg = args[i];
            if (typeof arg === 'string') {
                fragment.appendChild(ownerDocument.createTextNode(arg));
            } else {
                fragment.appendChild(arg);
            }
        }
        return fragment;
    },
    getInnerText: function(root) {
        var res = "";
        function walk(node) {
            if (node.nodeType === 3) { // TEXT_NODE
                res += node.nodeValue;
            } else if (node.nodeType === 1) { // ELEMENT_NODE
                var tag = node.tagName.toLowerCase();
                if (tag === "script" || tag === "style" || tag === "head") return;

                if (tag === "br") {
                    res += "\n";
                    return;
                }

                var isBlock = ["p", "div", "h1", "h2", "h3", "h4", "h5", "h6", "li", "tr"].indexOf(tag) !== -1;

                /* Block start: add newline if needed */
                if (isBlock && res.length > 0 && !res.endsWith("\n")) {
                    res += "\n";
                }

                var child = node.firstChild;
                while (child) {
                    walk(child);
                    child = child.nextSibling;
                }

                /* Block end: add newline if needed */
                if (isBlock && !res.endsWith("\n")) {
                    res += "\n";
                }
            }
        }
        walk(root);
        return res.trim();
    }
};
